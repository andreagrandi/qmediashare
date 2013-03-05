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

#include "flickrsession.h"

#include <QSettings>

FlickrSession::FlickrSession(const QString& token, const QString& nsid, const QString& userName, const QString& fullName,
				const QString& perms)
	: m_token(token),
	  m_nsid(nsid),
	  m_userName(userName),
	  m_fullName(fullName),
	  m_perms(perms)
{
}

FlickrSession::FlickrSession(QSettings* settings)
{
	settings->beginGroup("FlickrSession");
	m_token = settings->value("token").toString();
	m_nsid = settings->value("nsid").toString();
	m_userName = settings->value("userName").toString();
	m_fullName = settings->value("fullName").toString();
	m_perms = settings->value("perms").toString();
	settings->endGroup();
}

void FlickrSession::save(QSettings* settings)
{
	settings->beginGroup("FlickrSession");
	settings->setValue("token", m_token);
	settings->setValue("nsid", m_nsid);
	settings->setValue("userName", m_userName);
	settings->setValue("fullName", m_fullName);
	settings->setValue("perms", m_perms);
	settings->endGroup();
}


