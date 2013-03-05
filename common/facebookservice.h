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

#ifndef FACEBOOKSERVICE_H
#define FACEBOOKSERVICE_H

#include "service.h"
#include "album.h"

#include <QList>
#include <QSettings>
#include <QDialog>

class FacebookClient;
class FacebookSession;
class FacebookFriend;
class ImageTagger;
class Ui_FacebookLogin;

class FacebookService : public Service
{
public:
	FacebookService();
	FacebookService(QSettings* settings);
	~FacebookService();

	bool login();
	QList<Album*> getAlbums();
	Album* createAlbum(const QString& name);
	qulonglong uploadPhoto(Album* album, const QImage& image, const QString& fileName = "photo.jpg");
	void prepareForTagging();
	void tagPhoto(qulonglong photoId, const QImage& image);
	
	void abort();
	
	bool isError() const;
	bool requestAborted() const;

	QString name() const { return "Facebook (" + m_name + ")"; }
	QString serviceName() const { return "Facebook"; }
	QString type() const { return "FacebookService"; }
	bool supportsTagging() const { return true; }

	void save(QSettings* settings);

private:
	QString m_name;
	
	FacebookClient* m_client;
	FacebookSession* m_session;
	QDialog* m_loginDialog;
	Ui_FacebookLogin* m_loginUi;
	
	QList<FacebookFriend> m_friends;
};

#endif
