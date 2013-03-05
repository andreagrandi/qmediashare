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

#ifndef GPICASASERVICE_H
#define GPICASASERVICE_H

// GData uses HTTPS which is not available before Qt 4.3
#include <QtGlobal>
#if QT_VERSION >= 0x040300
#define HAS_GDATA

#include "service.h"
#include "ui_gdatalogin.h"

#include <QList>

class Album;
class QSettings;
class GPicasaClient;
class GDataSession;
class QDialog;

class GPicasaService : public Service
{
public:
	GPicasaService();
	GPicasaService(QSettings* settings);
	~GPicasaService();
	
	bool login();
	QList<Album*> getAlbums() { return QList<Album*>(); }
        Album* createAlbum(const QString& /*name*/) { return NULL; }
        qulonglong uploadPhoto(Album* /*album*/, const QImage& /*image*/, const QString& /*fileName = "photo.jpg"*/) { return 0; }
	
	void abort() {}
	
	bool isError() const;
	bool requestAborted() const { return false; }
	
	QString name() const { return "Google Picasa Web"; }
	QString serviceName() const { return "Google Picasa Web"; }
	QString type() const { return "GPicasaService"; }
	bool supportsTagging() const { return false; }

	void save(QSettings* settings);

private:
	void init();
	
	GPicasaClient* m_client;
	GDataSession* m_session;
	QDialog* m_loginDialog;
	Ui_GDataLogin m_loginDialogUi;
};

#endif

#endif
