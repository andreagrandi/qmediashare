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

#ifndef FLICKRPHOTOSET_H
#define FLICKRPHOTOSET_H

#include "album.h"

class FlickrPhotoSet : public Album
{
public:
	FlickrPhotoSet(const QString& id, const QString& name, int count, bool pending);
	
	QString serviceName() const { return "FlickrService"; }

	// In Flickr, you can't just create an empty album.  When we create a new album
	// in greenphoto it exists purely on the client in a "Pending" state, and is
	// created here the first time a photo is uploaded to it.
	bool isPending() { return m_pending; }
	void setPending(bool pending) { m_pending = pending; }

private:
	bool m_pending;
};

#endif
