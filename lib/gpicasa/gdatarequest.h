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

#ifndef GDATAREQUEST_H
#define GDATAREQUEST_H

// GData uses HTTPS which is not available before Qt 4.3
#include <QtGlobal>
#if QT_VERSION >= 0x040300
#define HAS_GDATA

#include "restrequest.h"
#include <QDomElement>
#include <QUrl>

class GDataSession;

class GDataRequest : public RestRequest
{
	Q_OBJECT
public:
	GDataRequest(GDataSession* session, const QUrl& url, bool xmlResponse = true);
	~GDataRequest();

	void start();
	QDomElement responseElement() const { return m_responseElement; }
	QString responseString() const { return m_responseString; }

signals:
	void complete(bool error);

private:
	void parseResponse(const QString& data);
	void errorResponse();

private:
	GDataSession* m_session;

	bool m_xmlResponse;
	QDomElement m_responseElement;
	QString m_responseString;
};

#endif

#endif
