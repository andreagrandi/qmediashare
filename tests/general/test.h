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

#ifndef TEST_H
#define TEST_H

#include "ui_test.h"
#include "album.h"

#include <QDialog>
#include <QSettings>

class Service;

class Test : public QDialog
{
	Q_OBJECT
public:
	Test(QDialog* parent);
	~Test();
	
private slots:
	void newFacebook();
	void newFlickr();
	void newPicasa();
	void remove();
	void getAlbums();
	void uploadPhoto();
	void createAlbum();

private:
	void saveAccounts();
	void loadAccounts();
	
	void newService(Service* service);
	
	Ui_Test m_ui;
	QSettings* m_settings;
	QList<Service*> m_services;
	QList<Album*> m_albums;
};

#endif
