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

#ifndef FLICKRCLIENT_H
#define FLICKRCLIENT_H

#include "flickrphotoset.h"
#include "flickrcomment.h"

#include <QVariant>

class QEventLoop;

class FlickrSession;
class FlickrRequest;

class FlickrClient : public QObject
{
	Q_OBJECT
public:
	FlickrClient();
	~FlickrClient();

	static QString apiKey() { return m_apiKey; }
	static QString secret() { return m_secret; }
	static void setApiKey(const QString& key) { m_apiKey = key; }
	static void setSecret(const QString& secret) { m_secret = secret; }
	
	FlickrSession* getFullToken(const QString& miniToken);
	QList<Album*> getPhotoSets(FlickrSession* session);
	QString uploadPhoto(FlickrSession* session, const QString& fileName, QImage photo);
	void createPhotoSet(FlickrSession* session, const QString& photoId, FlickrPhotoSet* photoSet);
	void addPhotoToSet(FlickrSession* session, const QString& photoId, const FlickrPhotoSet* photoSet);
	void setTags(FlickrSession* session, const QString& photoId, const QString& tags);
	void deletePhoto(FlickrSession* session, const QString& photoId);
	void addTags(FlickrSession* session, const QString& photoId, const QString& tags);
	QString addComment(FlickrSession* session, const QString& photoId, const QString& comment);
	void deleteComment(FlickrSession* session, const QString& commentId);
	void editComment(FlickrSession* session, const QString& commentId, const QString& comment);
	QList<FlickrComment*> getPhotoComments(FlickrSession* session, const QString& photoId);
	
	void abort();
	
	QString errorMessage() { return m_errorMessage; }
	int errorCode() { return m_errorCode; }
	bool requestAborted() { return m_aborted; }

signals:
	void uploadProgress(int bytes, int totalBytes);

private slots:
	void getFullTokenResponse(bool error);
	void getPhotoSetsResponse(bool error);
	void uploadPhotoResponse(bool error);
	void createPhotoSetResponse(bool error);
	void addPhotoToSetResponse(bool error);
	void setTagsResponse(bool error);
	void deletePhotoResponse(bool error);
	void addTagsResponse(bool error);
	void addCommentResponse(bool error);
	void deleteCommentResponse(bool error);
	void editCommentResponse(bool error);
	void getPhotoCommentsResponse(bool error);

private:
	void setErrorCode();
	
	static QString m_apiKey;
	static QString m_secret;

	FlickrRequest* m_request;
	QEventLoop* m_eventLoop;
	
	QVariant m_returnValue;
	void* m_returnPointer;
	
	QString m_errorMessage;
	int m_errorCode;
	bool m_aborted;
};

#endif
