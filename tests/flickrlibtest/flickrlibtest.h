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

#ifndef FLICKRLIBTEST_H
#define FLICKRLIBTEST_H

#include <QString>
#include <QSettings>

#include "flickrclient.h"
#include "flickrsession.h"
#include "flickrcomment.h"

class FlickrlibTest : QObject
{
	Q_OBJECT

	public:
		FlickrlibTest(const QString& miniToken);
		~FlickrlibTest();
		void hello();
		QString addComment(int photoId, const QString& comment);
		void deleteComment(const QString& commentId);
		void editComment(const QString& commentId, const QString& comment);
		QList<FlickrComment*> getPhotoComments(const QString& photoId);
	private:
		void saveSession();
		
		QSettings* m_settings;
		FlickrClient* m_client;
		FlickrSession* m_session;
};

#endif
