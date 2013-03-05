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

#include "flickrclient.h"
#include "flickrsession.h"
#include "flickrrequest.h"

#include <QEventLoop>
#include <QImage>
#include <QBuffer>
#include <QDebug>

/* The remote operations in this file follow a common pattern:
    1) Check there's not already an operation running.
    2) Construct a FlickrRequest object.
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

QString FlickrClient::m_apiKey;
QString FlickrClient::m_secret;

FlickrClient::FlickrClient()
	: m_request(NULL),
	  m_errorCode(0)
{
	m_eventLoop = new QEventLoop(this);
}


FlickrClient::~FlickrClient()
{
}

FlickrSession* FlickrClient::getFullToken(const QString& miniToken)
{
	if (m_eventLoop->isRunning())
		return NULL;
	
	m_request = new FlickrRequest(NULL, "flickr.auth.getFullToken");
	m_request->setArgument("mini_token", miniToken);

	connect(m_request, SIGNAL(complete(bool)), SLOT(getFullTokenResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();
	
	m_eventLoop->exec();
	m_request->deleteLater();
	
	return (FlickrSession*) m_returnPointer;
}

void FlickrClient::getFullTokenResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnPointer = NULL;
	else
	{
		QString token = m_request->response().firstChildElement("token").text();
		QString perms = m_request->response().firstChildElement("perms").text();
		QDomElement user = m_request->response().firstChildElement("user");
		QString nsid = user.attribute("nsid");
		QString userName = user.attribute("username");
		QString fullName = user.attribute("fullname");
		
		m_returnPointer = new FlickrSession(token, nsid, userName, fullName, perms);
	}
	
	m_eventLoop->exit();
}

QList<Album*> FlickrClient::getPhotoSets(FlickrSession* session)
{
	if (m_eventLoop->isRunning())
		return QList<Album*>();
	
	m_request = new FlickrRequest(session, "flickr.photosets.getList");
	
	connect(m_request, SIGNAL(complete(bool)), SLOT(getPhotoSetsResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
	
	if (m_returnPointer == NULL)
		return QList<Album*>();
	
	QList<Album*> ret(* ((QList<Album*>*)m_returnPointer));
	delete (QList<Album*>*)m_returnPointer;
	
	return ret;
}

void FlickrClient::getPhotoSetsResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnPointer = NULL;
	else
	{
		QList<Album*>* albums = new QList<Album*>;
		m_returnPointer = albums;
		
		QDomNodeList nodes = m_request->response().elementsByTagName("photoset");
		for (int i=0 ; i<nodes.count() ; ++i)
		{
			QDomElement photoset = nodes.at(i).toElement();
			QString id = photoset.attribute("id");
			QString name = photoset.firstChildElement("title").text();
			int count = photoset.attribute("photos").toInt();
			
			albums->append(new FlickrPhotoSet(id, name, count, false));
		}
	}
	
	m_eventLoop->exit();
}

QString FlickrClient::uploadPhoto(FlickrSession* session, const QString& fileName, QImage photo)
{
	if (m_eventLoop->isRunning())
		return NULL;

	if (photo.width() > 604 || photo.height() > 604)
		photo = photo.scaled(604, 604, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	QByteArray imageData;
	QBuffer buffer(&imageData);
	buffer.open(QIODevice::WriteOnly);
	photo.save(&buffer, "JPG", 80);
	buffer.close();
	
	m_request = new FlickrRequest(session, QString::null, "upload");
	m_request->setMimeData(fileName, "image/jpg", imageData, "photo");
	
	connect(m_request, SIGNAL(complete(bool)), SLOT(uploadPhotoResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
	
	return m_returnValue.toString();
}

void FlickrClient::uploadPhotoResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnValue = QVariant();
	else
		m_returnValue = m_request->response().text();
	
	m_eventLoop->exit();
}

void FlickrClient::addPhotoToSet(FlickrSession* session, const QString& photoId, const FlickrPhotoSet* photoSet)
{
	if (m_eventLoop->isRunning())
		return;
	
	m_request = new FlickrRequest(session, "flickr.photosets.addPhoto");
	m_request->setArgument("photo_id", photoId);
	m_request->setArgument("photoset_id", photoSet->id());
	
	connect(m_request, SIGNAL(complete(bool)), SLOT(addPhotoToSetResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
}

void FlickrClient::addPhotoToSetResponse(bool)
{
	setErrorCode();
	m_eventLoop->exit();
}

void FlickrClient::createPhotoSet(FlickrSession* session, const QString& photoId, FlickrPhotoSet* photoSet)
{
	if (m_eventLoop->isRunning())
		return;
	
	m_request = new FlickrRequest(session, "flickr.photosets.create");
	m_request->setArgument("title", photoSet->name());
	m_request->setArgument("primary_photo_id", photoId);
	
	connect(m_request, SIGNAL(complete(bool)), SLOT(createPhotoSetResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
	
	if (errorCode() != 0)
	{
		photoSet->setId(m_returnValue.toInt());
		photoSet->setPending(false);
		photoSet->setCount(1);
	}
}

void FlickrClient::createPhotoSetResponse(bool error)
{
	setErrorCode();
	
	if (!error)
		m_returnValue = m_request->response().firstChildElement("photoset").attribute("id");
	
	m_eventLoop->exit();
}

void FlickrClient::setErrorCode()
{
	m_errorCode = m_request->errorCode();
	m_errorMessage = m_request->errorMessage();
	m_aborted = m_request->aborted();
}

void FlickrClient::abort()
{
	if (!m_eventLoop->isRunning())
		return;
	
	m_request->abort();
}

void FlickrClient::setTags(FlickrSession* session, const QString& photoId, const QString& tags)
{
	if (m_eventLoop->isRunning())
		return;
	
	m_request = new FlickrRequest(session, "flickr.photos.setTags");
	m_request->setArgument("tags", tags);
	m_request->setArgument("photo_id", photoId);

	connect(m_request, SIGNAL(complete(bool)), SLOT(setTagsResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
}

void FlickrClient::setTagsResponse(bool)
{
	setErrorCode();
	m_eventLoop->exit();
}

void FlickrClient::deletePhoto(FlickrSession* session, const QString& photoId)
{
	if (m_eventLoop->isRunning())
		return;
	
	m_request = new FlickrRequest(session, "flickr.photos.delete");
	m_request->setArgument("photo_id", photoId);

	connect(m_request, SIGNAL(complete(bool)), SLOT(deletePhotoResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
}

void FlickrClient::deletePhotoResponse(bool)
{
	setErrorCode();	
	m_eventLoop->exit();
}

void FlickrClient::addTags(FlickrSession* session, const QString& photoId, const QString& tags)
{
	if (m_eventLoop->isRunning())
		return;
	
	m_request = new FlickrRequest(session, "flickr.photos.addTags");
	m_request->setArgument("tags", tags);
	m_request->setArgument("photo_id", photoId);

	connect(m_request, SIGNAL(complete(bool)), SLOT(addTagsResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
}

void FlickrClient::addTagsResponse(bool)
{
	setErrorCode();	
	m_eventLoop->exit();
}

QString FlickrClient::addComment(FlickrSession* session, const QString& photoId, const QString& comment)
{
	if (m_eventLoop->isRunning())
		return "";

	m_request = new FlickrRequest(session, "flickr.photos.comments.addComment");
	m_request->setArgument("comment_text", comment);
	m_request->setArgument("photo_id", photoId);

	connect(m_request, SIGNAL(complete(bool)), SLOT(addCommentResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();

	return m_returnValue.toString();
}

void FlickrClient::addCommentResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnValue = QVariant();
	else
	{
		m_returnValue = m_request->response().attribute("id");
	}

	m_eventLoop->exit();
}

void FlickrClient::deleteComment(FlickrSession* session, const QString& commentId)
{
	if (m_eventLoop->isRunning())
		return;

	m_request = new FlickrRequest(session, "flickr.photos.comments.deleteComment");
	m_request->setArgument("comment_id", commentId);

	connect(m_request, SIGNAL(complete(bool)), SLOT(deleteCommentResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
}

void FlickrClient::deleteCommentResponse(bool)
{
	setErrorCode();
	m_eventLoop->exit();
}

void FlickrClient::editComment(FlickrSession* session, const QString& commentId, const QString& comment)
{
	if (m_eventLoop->isRunning())
		return;

	m_request = new FlickrRequest(session, "flickr.photos.comments.editComment");
	m_request->setArgument("comment_id", commentId);
	m_request->setArgument("comment_text", comment);

	connect(m_request, SIGNAL(complete(bool)), SLOT(editCommentResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
}

void FlickrClient::editCommentResponse(bool)
{
	setErrorCode();
	m_eventLoop->exit();
}

QList<FlickrComment*> FlickrClient::getPhotoComments(FlickrSession* session, const QString& photoId)
{
	if (m_eventLoop->isRunning())
		return QList<FlickrComment*>();
	
	m_request = new FlickrRequest(session, "flickr.photos.comments.getList");
	m_request->setArgument("photo_id", photoId);
	
	connect(m_request, SIGNAL(complete(bool)), SLOT(getPhotoCommentsResponse(bool)));
	connect(m_request, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
	m_request->start();

	m_eventLoop->exec();
	m_request->deleteLater();
	
	if (m_returnPointer == NULL)
		return QList<FlickrComment*>();
	
	QList<FlickrComment*> ret(* ((QList<FlickrComment*>*)m_returnPointer));
	delete (QList<FlickrComment*>*)m_returnPointer;
	
	return ret;
}

void FlickrClient::getPhotoCommentsResponse(bool error)
{
	setErrorCode();
	if (error)
		m_returnPointer = NULL;
	else
	{
		QList<FlickrComment*>* comments = new QList<FlickrComment*>;
		m_returnPointer = comments;
		
		QDomNodeList nodes = m_request->response().elementsByTagName("comment");
		for (int i=0 ; i<nodes.count() ; ++i)
		{
			QDomElement comment = nodes.at(i).toElement();

			QString id = comment.attribute("id");
			QString author = comment.attribute("author");
			QString authorName = comment.attribute("authorname");
			QString dateCreate = comment.attribute("datecreate");
			QString permaLink = comment.attribute("permalink");
			QString commentText = comment.text();

			FlickrComment* cm; 
			cm = new FlickrComment(id, author, authorName, dateCreate, permaLink, commentText);

			comments->append(cm);
		}
	}
	
	m_eventLoop->exit();
}
