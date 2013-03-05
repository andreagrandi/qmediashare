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

#ifndef FLICKRREQUEST_H
#define FLICKRREQUEST_H

#include "restrequest.h"

#include <QDomElement>

class FlickrSession;

class FlickrRequest : public RestRequest
{
	Q_OBJECT

public:
	FlickrRequest(const FlickrSession* session, const QString& method, const QString& service = "rest");
	~FlickrRequest();
	
	void start();
	
	QDomElement response() const { return m_response; }

private:
	QString createSignature() const;
	void parseResponse(const QString& data);
	void errorResponse();
	
	QDomElement m_response;
	
};

#endif
