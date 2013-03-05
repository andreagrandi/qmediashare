/***************************************************************************
 *   Copyright (C) 2007 by David Sansome <davidsansome@gmail.com>          *
 *                         Andrea Grandi <a.grandi@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "restrequest.h"
#include "flickrexception.h"

#include <QHttp>
#include <QCoreApplication>
#include <QDebug>

RestRequest::RestRequest()
	: m_errorCode(0),
	  m_aborted(false),
	  m_redirecting(false)
{
}

RestRequest::RestRequest(const QUrl& endpoint)
	: m_errorCode(0),
	  m_aborted(false),
	  m_redirecting(false)
{
	setEndpoint(endpoint);
}

void RestRequest::setEndpoint(const QUrl& endpoint)
{
	// The endpoint is the URL all requests will go to.
	m_endpoint = endpoint;
	
#if QT_VERSION >= 0x040300
	// HTTPS is only supported on Qtopia >= 4.3
	if (m_endpoint.scheme() == "https")
		m_http = new QHttp(m_endpoint.host(), QHttp::ConnectionModeHttps, 443, this);
	else
#endif
		m_http = new QHttp(m_endpoint.host(), 80, this);

	connect(m_http, SIGNAL(requestFinished(int, bool)), SLOT(requestFinished(int, bool)));
	connect(m_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)), SLOT(responseHeaderReceived(const QHttpResponseHeader&)));
	connect(m_http, SIGNAL(dataSendProgress(int, int)), SIGNAL(uploadProgress(int, int)));
}

void RestRequest::setArgument(const QString& name, const QString& value)
{
	m_attributes[name] = value;
}

void RestRequest::setArgument(const QString& name, int value)
{
	setArgument(name, QString::number(value));
}

void RestRequest::setArgument(const QString& name, uint value)
{
	setArgument(name, QString::number(value));
}

void RestRequest::setArgument(const QString& name, qulonglong value)
{
	setArgument(name, QString::number(value));
}

void RestRequest::setHeader(const QString& name, const QString& value)
{
	m_headers[name] = value;
}

void RestRequest::setMimeData(const QString& filename, const QString& mimetype, const QByteArray& data, const QString& name)
{
	m_mimeFilename = filename;
	m_mimeType = mimetype;
	m_mimeData = data;
	m_mimeName = name;
}

void RestRequest::start()
{
	// Set our standard HTTP headers
	QHttpRequestHeader header("POST", m_endpoint.path());
	header.setValue("Host", m_endpoint.host());
	header.setValue("User-Agent", "RestClient C++ library (" + QCoreApplication::applicationName() + ")");
	
	// Add any user-specified HTTP headers
	QMapIterator<QString, QString> i(m_headers);
	while (i.hasNext())
	{
		i.next();
		header.setValue(i.key(), i.value());
	}
	
	QByteArray data;
	
	if (m_mimeFilename.isNull())
	{
		// We're not sending a file along with the request
		header.setValue("Content-type", "application/x-www-form-urlencoded");

		// Add all the arguments to the query
		QString postData;
		QMapIterator<QString, QString> it(m_attributes);
		while (it.hasNext())
		{
			it.next();
			if (!postData.isEmpty()) postData += "&";
			postData += escapePost(it.key()) + "=" + escapePost(it.value());
		}
		data = postData.toAscii();
	}
	else
	{
		// We're sending a file along with the request, setup a mime
		// multipart message.
		
		QString boundary = boundaryString(); // Long unique string
		header.setValue("Content-type", "multipart/form-data; boundary=" + boundary);
		header.setValue("MIME-version", "1.0");

		QString postData = "This is a multi-part message in MIME format.\r\n";
		postData += "--" + boundary + "\r\n";

		// Add our attributes
		QMapIterator<QString, QString> it(m_attributes);
		while (it.hasNext())
		{
			it.next();
			postData += "Content-Disposition: form-data; name=\"" + it.key() + "\"\r\n\r\n";
			postData += it.value() + "\r\n";
			postData += "--" + boundary + "\r\n";
		}
		postData += "Content-Disposition: form-data; filename=\"" + m_mimeFilename + "\"";
		
		// Add our data
		if (!m_mimeName.isNull())
			postData += "; name=" + m_mimeName;
		
		postData += "\r\nContent-type: " + m_mimeType + "\r\n\r\n";

		data = postData.toAscii();
		data.append(m_mimeData);
		data.append("\r\n--" + boundary + "\r\n");
	}

	// Do the request
	qDebug() << data;
	m_http->request(header, data);
}

void RestRequest::abort()
{
	m_http->abort();
	m_aborted = true;
}

QString RestRequest::escapePost(const QString& input)
{
	QString ret = QUrl::toPercentEncoding(input);
	return ret.replace(" ", "+");
}

QString RestRequest::boundaryString() const
{
	// Mime multipart messages need to be seperated by a unique string that
	// doesn't exist in the data being sent.  This function generates
	// such a string.
	
	char min = 'A';
	char max = 'z';
	
	QString stuff;
	
	do
	{
		stuff.clear();
		for (int i=0 ; i<40 ; ++i)
			stuff += QLatin1Char((qrand() % (max-min)) + min);
	}
	while (m_mimeData.contains(stuff.toAscii()));

	return stuff;
}

void RestRequest::requestFinished(int id, bool error)
{
	Q_UNUSED(id)
	if (m_redirecting)
	{
		m_redirecting = false;
		return;
	}
	
	if (error)
	{
		m_errorCode = m_http->error();
		m_errorMessage = m_http->errorString();
		errorResponse();
	}
	else
	{
		parseResponse(m_http->readAll());
	}
}

void RestRequest::responseHeaderReceived(const QHttpResponseHeader& resp)
{
	// Handle HTTP redirects like you get with Google Accounts login
	if (resp.statusCode() == 301 || resp.statusCode() == 302)
	{
		m_redirecting = true;
		disconnect(m_http);
		m_http->deleteLater();
		
		setEndpoint(QUrl(resp.value("location")));
		start();
	}
}


