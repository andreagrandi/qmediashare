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

#ifndef ALBUM_H
#define ALBUM_H

#include <QString>

class Album
{
public:
	Album(const QString& id, const QString& name, int count);
	virtual ~Album() {}
	
	virtual QString serviceName() const = 0;
	
	QString id() const { return m_id; }
	QString name() const { return m_name; }
	int count() const { return m_count; }
	
	void setId(int id) { m_id = id; }
	void setCount(int count) { m_count = count; }

private:
	QString m_id;
	QString m_name;
	int m_count;
};

#endif
