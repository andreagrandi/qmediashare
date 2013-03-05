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

#ifndef FACEBOOKCLIENT_H
#define FACEBOOKCLIENT_H

#include <QEventLoop>
#include <QVariant>
#include <QPair>

#include "facebookalbum.h"

typedef QPair<QPoint, QVariant> FacebookTag;

class FacebookRequest;
class FacebookSession;
class FacebookFriend;

class FacebookClient : public QObject
{
	Q_OBJECT
public:
	FacebookClient();
	~FacebookClient();
	
	static QString apiKey() { return m_apiKey; }
	static QString secret() { return m_secret; }
	static void setApiKey(const QString& key) { m_apiKey = key; }
	static void setSecret(const QString& secret) { m_secret = secret; }
	
	FacebookSession* getSession(const QString& authToken);
	QString getName(FacebookSession* session);
	QList<FacebookFriend> getFriends(FacebookSession* session);
	QList<Album*> getAlbums(FacebookSession* session);
	Album* createAlbum(FacebookSession* session, const QString& name);
	qulonglong uploadPhoto(FacebookSession* session, const FacebookAlbum* album, QImage photo);
	void addTags(FacebookSession* session, qulonglong pid, QList<FacebookTag> tags);
	
	void abort();

	QString errorMessage() { return m_errorMessage; }
	int errorCode() { return m_errorCode; }
	bool requestAborted() { return m_aborted; }

signals:
	void uploadProgress(int bytes, int totalBytes);

private slots:
	void getSessionResponse(bool error);
	void getAlbumsResponse(bool error);
	void getNameResponse(bool error);
	void createAlbumResponse(bool error);
	void uploadPhotoResponse(bool error);
	void addTagsResponse(bool error);
	void getFriendsResponse(bool error);

private:
	void setErrorCode();

	static QString m_apiKey;
	static QString m_secret;
	
	FacebookRequest* m_request;
	QEventLoop* m_eventLoop;
	
	QVariant m_returnValue;
	void* m_returnPointer;

	QString m_errorMessage;
	int m_errorCode;
	bool m_aborted;
};

#endif
