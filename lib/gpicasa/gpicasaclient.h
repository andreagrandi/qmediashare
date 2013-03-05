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

#ifndef GPICASACLIENT_H
#define GPICASACLIENT_H

// GData uses HTTPS which is not available before Qt 4.3
#include <QtGlobal>
#if QT_VERSION >= 0x040300
#define HAS_GDATA

#include <QEventLoop>
#include <QVariant>

class GDataSession;
class GDataRequest;

class GPicasaClient : public QObject
{
	Q_OBJECT
public:
	GPicasaClient();
	~GPicasaClient();
	
	static QString version() { return m_version; }
	static void setVersion(const QString& version) { m_version = version; }
	
	GDataSession* login(const QString& email, const QString& password);
	//QList<Album*> getAlbums(FacebookSession* session);
	
	QString errorMessage() { return m_errorMessage; }
	int errorCode() { return m_errorCode; }
	
private slots:
	void loginResponse(bool error);

private:
	void setErrorCode();
	static QString sourceString();
	
	static QString m_version;
	
	GDataRequest* m_request;
	QEventLoop* m_eventLoop;
	
	QVariant m_returnValue;
	void* m_returnPointer;

	QString m_errorMessage;
	int m_errorCode;
};

#endif

#endif
