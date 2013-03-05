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

#include "gdatasession.h"

GDataSession::GDataSession(const QString& auth)
	: m_auth(auth)
{
}

GDataSession::GDataSession(QSettings* settings)
{
	settings->beginGroup("GDataSession");
	m_auth = settings->value("auth").toString();
	settings->endGroup();
}

void GDataSession::save(QSettings* settings)
{
	settings->beginGroup("GDataSession");
	settings->setValue("auth", m_auth);
	settings->endGroup();
}


#endif


