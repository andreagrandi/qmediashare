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

#include "gpicasaclient.h"
#include "gdatarequest.h"
#include "gdatasession.h"

#include <QCoreApplication>
#include <QStringList>

QString GPicasaClient::m_version;

GPicasaClient::GPicasaClient()
	: m_request(NULL),
	  m_errorCode(0)
{
	m_eventLoop = new QEventLoop(this);
}

GPicasaClient::~GPicasaClient()
{
}

QString GPicasaClient::sourceString()
{
	return QCoreApplication::organizationName() + "-" +
	       QCoreApplication::applicationName() + "-" +
	       m_version;
}

GDataSession* GPicasaClient::login(const QString& email, const QString& password)
{
	if (m_eventLoop->isRunning())
		return NULL;
	
	m_request = new GDataRequest(NULL, QUrl("https://www.google.com/accounts/ClientLogin"), false);
	m_request->setArgument("accountType", "HOSTED_OR_GOOGLE");
	m_request->setArgument("Email", email);
	m_request->setArgument("Passwd", password);
	m_request->setArgument("service", "lh2");
	m_request->setArgument("source", sourceString());
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(loginResponse(bool)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	return (GDataSession*) m_returnPointer;
}

void GPicasaClient::loginResponse(bool error)
{
	setErrorCode();
	m_returnPointer = NULL;
	
	if (!error)
	{
		QStringList lines = m_request->responseString().split("\n");
		
		foreach (QString line, lines)
		{
			if (!line.startsWith("Auth="))
				continue;
			
			QString auth = line.right(line.length() - 5);
			m_returnPointer = new GDataSession(auth);
		}
	}
	
	m_eventLoop->exit();
}

/*QList<Album*> GPicasaClient::getAlbums(FacebookSession* session)
{
	if (m_eventLoop->isRunning())
		return NULL;
	
	m_request = new GDataRequest(NULL, QUrl("https://www.google.com/accounts/ClientLogin"), false);
	m_request->setArgument("accountType", "HOSTED_OR_GOOGLE");
	m_request->setArgument("Email", email);
	m_request->setArgument("Passwd", password);
	m_request->setArgument("service", "lh2");
	m_request->setArgument("source", sourceString());
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(loginResponse(bool)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	return (GDataSession*) m_returnPointer;
}*/

void GPicasaClient::setErrorCode()
{
	m_errorCode = m_request->errorCode();
	m_errorMessage = m_request->errorMessage();
}

#endif



