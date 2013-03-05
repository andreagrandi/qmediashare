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

#include "facebookrequest.h"
#include "facebooksession.h"
#include "facebookclient.h"
#include "md5.h"

#include <QDebug>
#include <QUrl>
#include <QDomDocument>
#include <QCoreApplication>

#include <sys/time.h>
#include <time.h>

FacebookRequest::FacebookRequest(FacebookSession* session, const QString& method)
	: RestRequest(QUrl("http://api.facebook.com/restserver.php")),
	  m_session(session)
{
	setArgument("api_key", FacebookClient::apiKey());
	setArgument("v", "1.0");
	setArgument("method", method);
	if (session && !session->key().isNull())
		setArgument("session_key", session->key());
}


FacebookRequest::~FacebookRequest()
{
}

QString FacebookRequest::createSignature() const
{
	// Facebook requires a signature to be sent with each request.
	// This is the md5sum of each of the key=value attribute pairs
	// concatenated together with the client's secret key.
	QString requestStr;
	
	QMapIterator<QString, QString> it(m_attributes);
	while (it.hasNext())
	{
		it.next();

		if (it.key() == "sig")
			continue;
		
		requestStr += it.key() + "=" + it.value();
	}

	if (m_session && !m_session->secret().isNull())
		requestStr += m_session->secret();
	else
		requestStr += FacebookClient::secret();

	md5_byte_t result[16];
	md5_state_t state;
	md5_init(&state);
	md5_append(&state, (md5_byte_t*) requestStr.toAscii().data(), requestStr.length());
	md5_finish(&state, result);

	QString signature;
	for (int i=0 ; i<16 ; ++i)
	{
		QString hex = QString::number(result[i], 16);
		if (hex.length() == 1)
			hex = "0" + hex;
		
		signature += hex;
	}

	return signature;
}

float FacebookRequest::callId()
{
	// The id of each call must be greater than the last
	timeval tv;
	gettimeofday(&tv, NULL);
	
	return (float(tv.tv_sec) + float(tv.tv_usec) / 1000000.0f) * 100.0f;
}

void FacebookRequest::start()
{
	setArgument("call_id", QString::number(callId(), 'f', 0));
	setArgument("sig", createSignature());

	RestRequest::start();
}

void FacebookRequest::parseResponse(const QString& data)
{
	qDebug() << data;
	
	QDomDocument doc;
	doc.setContent(data);

	m_response = doc.documentElement();
	
	if (m_response.tagName() == "error_response")
	{
		m_errorCode = m_response.elementsByTagName("error_code").at(0).toElement().text().toInt();
		m_errorMessage = m_response.elementsByTagName("error_msg").at(0).toElement().text();
	}
	
	emit(complete(m_errorCode != 0));
}

void FacebookRequest::errorResponse()
{
	emit(complete(true));
}


