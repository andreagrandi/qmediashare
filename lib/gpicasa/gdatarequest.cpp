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

// GData uses HTTPS which is not available before Qt 4.3
#include <QtGlobal>
#if QT_VERSION >= 0x040300

#include <QDebug>

#include "gdatarequest.h"
#include "gdatasession.h"

GDataRequest::GDataRequest(GDataSession* session, const QUrl& url, bool xmlResponse)
	: RestRequest(url),
	  m_session(session),
	  m_xmlResponse(xmlResponse)
{
	if (session != NULL)
		setHeader("Authorization", "GoogleLogin auth=" + session->auth());
}

GDataRequest::~GDataRequest()
{
}

void GDataRequest::start()
{
	RestRequest::start();
}

void GDataRequest::parseResponse(const QString& data)
{
	qDebug() << data;
	
	if (m_xmlResponse)
	{
		QDomDocument doc;
		doc.setContent(data);
		m_responseElement = doc.documentElement();
	}
	else
		m_responseString = data;
	
	emit(complete(m_errorCode != 0));
}

void GDataRequest::errorResponse()
{
	emit(complete(true));
}

#endif



