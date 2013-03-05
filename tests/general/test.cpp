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

#include "test.h"
#include "facebookservice.h"
#include "flickrservice.h"
#include "gpicasaservice.h"

#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>

// This is a Desktop Qt client for playing with libgreenphoto
// without a Greenphone or QVFB.

Test::Test(QDialog* parent)
	: QDialog(parent)
{
	m_ui.setupUi(this);
	
	m_ui.albumList->addItem("(Default album)");
	
	connect(m_ui.getAlbums, SIGNAL(clicked()), SLOT(getAlbums()));
	connect(m_ui.newFacebook, SIGNAL(clicked()), SLOT(newFacebook()));
	connect(m_ui.newFlickr, SIGNAL(clicked()), SLOT(newFlickr()));
	connect(m_ui.removeButton, SIGNAL(clicked()), SLOT(remove()));
	connect(m_ui.newPicasa, SIGNAL(clicked()), SLOT(newPicasa()));
	connect(m_ui.uploadPhoto, SIGNAL(clicked()), SLOT(uploadPhoto()));
	connect(m_ui.createAlbum, SIGNAL(clicked()), SLOT(createAlbum()));

	m_settings = new QSettings(this);
	loadAccounts();
}

Test::~Test()
{
	saveAccounts();
	
	foreach (Album* a, m_albums)
		delete a;
}

void Test::saveAccounts()
{
	m_settings->beginWriteArray("accounts", m_services.count());
	int i=0;
	foreach (Service* service, m_services)
	{
		m_settings->setArrayIndex(i++);
		m_settings->setValue("type", service->type());
		service->save(m_settings);
	}
	m_settings->endArray();
}

void Test::loadAccounts()
{
	foreach (Service* s, m_services)
		delete s;
	m_services.clear();
	
	int size = m_settings->beginReadArray("accounts");
	for (int i=0 ; i<size ; ++i)
	{
		m_settings->setArrayIndex(i);
		
		QString type = m_settings->value("type").toString();

		Service* service = NULL;
		if (type == "FacebookService")
			service = new FacebookService(m_settings);
		else if (type == "FlickrService")
			service = new FlickrService(m_settings);
#ifdef HAS_GDATA
		else if (type == "GPicasaService")
			service = new GPicasaService(m_settings);
#endif
		
		if (service != NULL)
		{
			m_services.append(service);
			m_ui.accountList->addItem(service->name());
		}
	}
	m_settings->endArray();
}

void Test::newFacebook()
{
	newService(new FacebookService());
}

void Test::newFlickr()
{
	newService(new FlickrService());
}

void Test::newPicasa()
{
#ifdef HAS_GDATA
	newService(new GPicasaService());
#else
	QMessageBox::information(this, "Google Picasa", "Qt 4.3 is required for Google Picasa Web support");
#endif
}

void Test::newService(Service* service)
{
	if (!service->login())
	{
		delete service;
		return;
	}

	m_services.append(service);
	m_ui.accountList->addItem(service->name());
	saveAccounts();
}

void Test::getAlbums()
{
	if (m_ui.accountList->currentRow() == -1)
		return;

	Service* service = m_services[m_ui.accountList->currentRow()];
	
	QList<Album*> albums = service->getAlbums();
	
	if (service->isError())
		return;
	
	foreach (Album* a, m_albums)
		delete a;
	m_albums = albums;
	m_ui.albumList->clear();
	m_ui.albumList->addItem("(Default album)");
	
	foreach (Album* a, m_albums)
	{
		m_ui.albumList->addItem(a->name() + " (" + QString::number(a->count()) + ")");
	}
}

void Test::uploadPhoto()
{
	if (m_ui.accountList->currentRow() == -1)
		return;

	Service* service = m_services[m_ui.accountList->currentRow()];
	Album* album = NULL;
	if (m_ui.albumList->currentRow() > 0)
		album = m_albums[m_ui.albumList->currentRow() - 1];

	QString fileName = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Images (*.png *.jpg *.jpeg)");
	if (fileName.isNull())
		return;

	QImage image(fileName);
	qulonglong id = service->uploadPhoto(album, image, fileName);
	
	if (service->supportsTagging())
		service->tagPhoto(id, image);
}

void Test::createAlbum()
{
	if (m_ui.accountList->currentRow() == -1)
		return;

	QString name = QInputDialog::getText(this, "Album name", "Enter a name for the new album");
	if (name.isNull())
		return;

	Service* service = m_services[m_ui.accountList->currentRow()];
	Album* album = service->createAlbum(name);
	
	if (!album)
		return;

	m_albums.append(album);
	m_ui.albumList->addItem(album->name() + " (" + QString::number(album->count()) + ")");
}

void Test::remove()
{
	int i = m_ui.accountList->currentRow();
	if (i == -1)
		return;
	
	QListWidgetItem* item = m_ui.accountList->takeItem(i);
	delete item;
	
	delete m_services[i];
	m_services.removeAt(i);
	
	saveAccounts();
}


