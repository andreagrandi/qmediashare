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

#include "flickrrequest.h"
#include "flickrclient.h"
#include "flickrsession.h"
#include "md5.h"

#include <QDebug>

FlickrRequest::FlickrRequest(const FlickrSession* session, const QString& method, const QString& service)
	: RestRequest(QUrl("http://api.flickr.com/services/" + service + "/"))
{
	if (!method.isEmpty())
		setArgument("method", method);
	
	setArgument("api_key", FlickrClient::apiKey());
	
	if (session != NULL)
		setArgument("auth_token", session->token());
}


FlickrRequest::~FlickrRequest()
{
}

void FlickrRequest::start()
{
	setArgument("api_sig", createSignature());
	
	RestRequest::start();
}

QString FlickrRequest::createSignature() const
{
	// Flickr requires us to send a signature along with each request.
	// This is the md5sum of all the attribute keyvalue pairs concatenated
	// together
	QString requestStr = FlickrClient::secret();
	
	QMapIterator<QString, QString> it(m_attributes);
	while (it.hasNext())
	{
		it.next();

		if (it.key() == "api_sig")
			continue;
		
		requestStr += it.key() + it.value();
	}
	
	qDebug() << requestStr;

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

void FlickrRequest::parseResponse(const QString& data)
{
	qDebug() << data;

	QDomDocument doc;
	doc.setContent(data);

	m_response = doc.documentElement().firstChild().toElement();
	
	if (doc.documentElement().attribute("stat") == "fail")
	{
		m_errorCode = m_response.attribute("code").toInt();
		m_errorMessage = m_response.attribute("msg");
	}
	
	emit(complete(m_errorCode != 0));
}

void FlickrRequest::errorResponse()
{
	emit(complete(true));
}




