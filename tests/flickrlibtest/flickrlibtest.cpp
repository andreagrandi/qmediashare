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

#include "flickrlibtest.h"
#include "flickrclient.h"

#include <QDebug>

FlickrlibTest::FlickrlibTest(const QString& miniToken)
{
	m_settings = new QSettings(this);
	m_client = new FlickrClient;

	if(m_settings->contains("FlickrSession/token"))
	{	
    	m_session = new FlickrSession(m_settings);
		qDebug() << m_session->token();
	}
	else
	{
		m_session = m_client->getFullToken(miniToken);

		if(m_session)
			m_session->save(m_settings);
	}
}

FlickrlibTest::~FlickrlibTest()
{
	saveSession();
}

void FlickrlibTest::saveSession()
{
	m_session->save(m_settings);
}

void FlickrlibTest::hello()
{
	qDebug("Hello!");
}

QString FlickrlibTest::addComment(int photoId, const QString& comment)
{
        QString strComment = m_client->addComment(m_session, QString().arg(photoId), comment);
	return strComment;
}

void FlickrlibTest::deleteComment(const QString& commentId)
{
	m_client->deleteComment(m_session, commentId);
}

void FlickrlibTest::editComment(const QString& commentId, const QString& comment)
{
	m_client->editComment(m_session, commentId, comment);
}

QList<FlickrComment*> FlickrlibTest::getPhotoComments(const QString& photoId)
{
        return m_client->getPhotoComments(m_session, photoId);
}
