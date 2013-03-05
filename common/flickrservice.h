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

#ifndef FLICKRSERVICE_H
#define FLICKRSERVICE_H

#include <QList>
#include <QImage>
#include <QDialog>

#include "album.h"
#include "flickrsession.h"
#include "service.h"

class FlickrClient;
class Ui_FlickrLogin;

class FlickrService : public Service
{
public:
	FlickrService();
	FlickrService(QSettings* settings);
	~FlickrService();
	
	bool login();
	QList<Album*> getAlbums();
	Album* createAlbum(const QString& name);
	qulonglong uploadPhoto(Album* album, const QImage& image, const QString& fileName = "photo.jpg");
	
	void abort();
	
	bool isError() const;
	bool requestAborted() const;

	QString name() const;
	QString serviceName() const { return "Flickr"; }
	QString type() const { return "FlickrService"; }
	bool supportsTagging() const { return false; }

	void save(QSettings* settings);

private:
	FlickrClient* m_client;
	FlickrSession* m_session;
	QDialog* m_loginDialog;
	Ui_FlickrLogin* m_loginUi;

};

#endif
