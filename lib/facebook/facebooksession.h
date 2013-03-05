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

#ifndef FACEBOOKSESSION_H
#define FACEBOOKSESSION_H

#include <QString>
#include <QSettings>

class FacebookSession
{
public:
	FacebookSession(const QString& k, const QString& s, const QString& u);
	FacebookSession(QSettings* settings);
	
	QString key() { return m_key; }
	QString secret() { return m_secret; }
	QString uid() { return m_uid; }

	void save(QSettings* settings);
	
private:
	QString m_key;
	QString m_secret;
	QString m_uid;
};

#endif
