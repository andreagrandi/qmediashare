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

#ifndef FLICKRCOMMENT_H
#define FLICKRCOMMENT_H

#include <QString>

class FlickrComment
{
	public:
		FlickrComment(const QString& id, const QString& author, const QString& authorname, const QString& datecreate,
						const QString& permalink, const QString& comment);
		~FlickrComment();

		QString Id() const { return m_id; }
		QString Author() const { return m_author; }
		QString AuthorName() const { return m_authorname; }
		QString DateCreate() const { return m_datecreate; }
		QString PermaLink() const { return m_permalink; }
		QString Comment() const { return m_comment; }
	private:
		QString m_id;
		QString m_author;
		QString m_authorname;
		QString m_datecreate;
		QString m_permalink;
		QString m_comment;
};

#endif
