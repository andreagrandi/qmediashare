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

#include "facebookclient.h"
#include "facebookrequest.h"
#include "facebooksession.h"
#include "facebookfriend.h"

#include <QImage>
#include <QBuffer>

/* The remote operations in this file follow a common pattern:
    1) Check there's not already an operation running.
    2) Construct a FacebookRequest object.
    3) Set any arguments to the request.
    4) Start the request.
    5) Start an event loop and wait until it exits.
       The event loop will be exited when the FacebookRequest
       emits its complete(bool) signal.
    6) Get the return object, cleanup, and return it

   The return objects are either stored in m_returnValue if they're
   simple types, or m_returnPointer if they're not.  Since it's bad
   to return pointers, we usually make a copy of the object and return that
   by value.  Since most Qt objects do shallow copies with reference counting,
   this is quite fast.
*/

QString FacebookClient::m_apiKey;
QString FacebookClient::m_secret;

FacebookClient::FacebookClient()
	: m_request(NULL),
	  m_errorCode(0)
{
	m_eventLoop = new QEventLoop(this);
}


FacebookClient::~FacebookClient()
{
}

FacebookSession* FacebookClient::getSession(const QString& authToken)
{
	if (m_eventLoop->isRunning())
		return NULL;
	
	m_request = new FacebookRequest(NULL, "facebook.auth.getSession");
	m_request->setArgument("auth_token", authToken);
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(getSessionResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	return (FacebookSession*) m_returnPointer;
}

void FacebookClient::getSessionResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnPointer = NULL;
	else
	{
		QString key = m_request->response().firstChildElement("session_key").text();
		QString secret = m_request->response().firstChildElement("secret").text();
		QString uid = m_request->response().firstChildElement("uid").text();
		
		m_returnPointer = new FacebookSession(key, secret, uid);
	}
	
	m_eventLoop->exit();
}

QString FacebookClient::getName(FacebookSession* session)
{
	if (m_eventLoop->isRunning())
		return NULL;
	
	m_request = new FacebookRequest(session, "facebook.fql.query");
	m_request->setArgument("query", "SELECT name FROM user WHERE uid=" + session->uid());
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(getNameResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	return m_returnValue.toString();
}

void FacebookClient::getNameResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnValue.clear();
	else
	{
		QString name = m_request->response().firstChildElement("user").firstChildElement("name").text();
		
		m_returnValue = name;
	}
	
	m_eventLoop->exit();
}

QList<Album*> FacebookClient::getAlbums(FacebookSession* session)
{
	if (m_eventLoop->isRunning())
		return QList<Album*>();
	
	m_request = new FacebookRequest(session, "facebook.fql.query");
	m_request->setArgument("query", "SELECT aid, name, created, description, size FROM album WHERE owner=" + session->uid());
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(getAlbumsResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	if (m_returnPointer == NULL)
		return QList<Album*>();
	
	QList<Album*> ret(* ((QList<Album*>*)m_returnPointer));
	delete (QList<Album*>*)m_returnPointer;
	
	return ret;
}

void FacebookClient::getAlbumsResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnPointer = NULL;
	else
	{
		QList<Album*>* albums = new QList<Album*>;
		m_returnPointer = albums;
		
		QDomNodeList nodes = m_request->response().elementsByTagName("album");
		for (int i=0 ; i<nodes.count() ; ++i)
		{
			QDomElement album = nodes.at(i).toElement();
			QString id = album.firstChildElement("aid").text();
			QString name = album.firstChildElement("name").text();
			int count = album.firstChildElement("size").text().toInt();
			QDateTime creationDate = QDateTime::fromTime_t(album.firstChildElement("created").text().toInt());
			
			albums->append(new FacebookAlbum(id, name, count, creationDate));
		}
	}
	
	m_eventLoop->exit();
}

Album* FacebookClient::createAlbum(FacebookSession* session, const QString& name)
{
	if (m_eventLoop->isRunning())
		return NULL;
	
	m_request = new FacebookRequest(session, "facebook.photos.createAlbum");
	m_request->setArgument("name", name);
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(createAlbumResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	return (Album*) m_returnPointer;
}

void FacebookClient::createAlbumResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnPointer = NULL;
	else
	{
		QDomElement r = m_request->response();
		
		QString id = r.firstChildElement("aid").text();
		QString name = r.firstChildElement("name").text();
		int count = r.firstChildElement("size").text().toInt();
		QDateTime creationDate = QDateTime::fromTime_t(r.firstChildElement("created").text().toInt());

		m_returnPointer = new FacebookAlbum(id, name, count, creationDate);
	}
	
	m_eventLoop->exit();
}

qulonglong FacebookClient::uploadPhoto(FacebookSession* session, const FacebookAlbum* album, QImage photo)
{
	if (m_eventLoop->isRunning())
		return 0;

	// Facebook requires photos to be at most 604x604 pixels
	if (photo.width() > 604 || photo.height() > 604)
		photo = photo.scaled(604, 604, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	QByteArray imageData;
	QBuffer buffer(&imageData);
	buffer.open(QIODevice::WriteOnly);
	photo.save(&buffer, "JPG", 80);
	buffer.close();
	
	m_request = new FacebookRequest(session, "facebook.photos.upload");
	if (album)
		m_request->setArgument("aid", album->id());
		
	m_request->setMimeData("photo.jpg", "image/jpg", imageData);
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(uploadPhotoResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	return m_returnValue.toULongLong();
}

void FacebookClient::uploadPhotoResponse(bool error)
{
	setErrorCode();
	
	if (error)
		m_returnValue = QVariant();
	else
	{
		QDomElement r = m_request->response();
		
		QString pid = r.firstChildElement("pid").text();
		m_returnValue.setValue(pid.toULongLong());
	}
	
	m_eventLoop->exit();
}

void FacebookClient::abort()
{
	if (!m_eventLoop->isRunning())
		return;
	
	m_request->abort();
}

void FacebookClient::setErrorCode()
{
	m_errorCode = m_request->errorCode();
	m_errorMessage = m_request->errorMessage();
	m_aborted = m_request->aborted();
}

void FacebookClient::addTags(FacebookSession* session, qulonglong pid, QList<FacebookTag> tags)
{
	if (m_eventLoop->isRunning())
		return;
	
	if (tags.count() == 0)
		return;
	
	// Build the tags list
	QString tagString = "[";
	foreach (FacebookTag tag, tags)
	{
		if (tagString.length() > 1)
			tagString += ",";
		
		// JSON
		tagString += "{\"x\":" + QString::number(tag.first.x()) + ","
			"\"y\":" + QString::number(tag.first.y()) + "," +
			(tag.second.type() == QVariant::String ?
				"\"tag_text\":\"" + tag.second.toString().replace("\\", "\\\\").replace("\"", "\\\"") + "\"" :
				"\"tag_uid\":" + QString::number(tag.second.toUInt())) +
			"}";
	}
	tagString += "]";
	
	m_request = new FacebookRequest(session, "facebook.photos.addTag");
	m_request->setArgument("pid", pid);
	m_request->setArgument("tags", tagString);
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(addTagsResponse(bool)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
}

void FacebookClient::addTagsResponse(bool)
{
	setErrorCode();
	m_eventLoop->exit();
}

QList<FacebookFriend> FacebookClient::getFriends(FacebookSession* session)
{
	if (m_eventLoop->isRunning())
		return QList<FacebookFriend>();
	
	m_request = new FacebookRequest(session, "fql.query");
	m_request->setArgument("query", "SELECT uid, name FROM user WHERE uid IN (SELECT uid2 FROM friend WHERE uid1=" + session->uid() + ")");
	m_request->start();
	connect(m_request, SIGNAL(complete(bool)), SLOT(getFriendsResponse(bool)));
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	QList<FacebookFriend> ret(* ((QList<FacebookFriend>*)m_returnPointer));
	delete (QList<FacebookFriend>*)m_returnPointer;
	
	return ret;
}

void FacebookClient::getFriendsResponse(bool error)
{
	setErrorCode();
	
	QList<FacebookFriend>* friends = new QList<FacebookFriend>;
	m_returnPointer = friends;
	
	if (!error)
	{
		QDomNodeList nodes = m_request->response().elementsByTagName("user");
		for (int i=0 ; i<nodes.count() ; ++i)
		{
			FacebookFriend user;
			user.setId(nodes.at(i).toElement().elementsByTagName("uid").at(0).toElement().text().toUInt());
			user.setName(nodes.at(i).toElement().elementsByTagName("name").at(0).toElement().text());
			
			*friends << user;
		}
	}
	
	m_eventLoop->exit();
}






