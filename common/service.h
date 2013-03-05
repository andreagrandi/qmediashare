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

#ifndef SERVICE_H
#define SERVICE_H

#include "album.h"

#include <QSettings>
#include <QObject>

// The Service class is the base class for all online image galleries.
// Individual galleries (Facebook, Flickr) reimplement this to provide
// functionality.  There is one Service object for each account.

class Service : public QObject
{
	Q_OBJECT
public:
	virtual ~Service() {}
	
	// Get the user's credentials and try to login.
	// Return true if successful.
	virtual bool login() = 0;
	
	// Get a list of albums that the user can upload photos to
	virtual QList<Album*> getAlbums() = 0;
	
	// Create a new album with the given name
	virtual Album* createAlbum(const QString& name) = 0;
	
	// Upload a photo to the given album.  Should return a photo ID on success.
	virtual qulonglong uploadPhoto(Album* album, const QImage& image, const QString& fileName = "photo.jpg") = 0;
	
	// Do anything that's required to tag photos, like get a list of the user's friends.
	virtual void prepareForTagging() {}
	
	// Start tagging a photo with the given ID.  This should show any required UI and
	// block until the tagging is complete.
	virtual void tagPhoto(qulonglong photoId, const QImage& image) {Q_UNUSED(photoId) Q_UNUSED(image)}
	
	// Stop any action that is currently in progress
	virtual void abort() = 0;
	
	// Called after each action to find out if anything went wrong
	virtual bool isError() const = 0;
	virtual bool requestAborted() const = 0;

	// Some information about the service
	virtual QString name() const = 0; // Friendly account name to be shown in the UI (eg "Facebook (Joe Bloggs)")
	virtual QString serviceName() const = 0; // Friendly name of the service (eg. "Facebook")
	virtual QString type() const = 0; // Unique name to identify this class.  Usually the same as the derived class' name
	virtual bool supportsTagging() const = 0; // If true, the button to tag photos will be enabled

	// Persists any account information.
	// The settings object will already be set to use a unique group.
	virtual void save(QSettings* settings) = 0;

signals:
	void uploadProgress(int bytes, int totalBytes);
};

#endif
