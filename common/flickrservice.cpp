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

#include "flickrservice.h"
#include "flickrclient.h"
#include "ui_flickrlogin.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QRegExpValidator>

#ifdef Q_WS_QWS
#include <QtopiaApplication>
#endif

FlickrService::FlickrService()
{
	m_session = NULL;
	m_client = new FlickrClient;
	m_loginDialog = NULL;
	
	connect(m_client, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
}

FlickrService::FlickrService(QSettings* settings)
{
	m_session = new FlickrSession(settings);
	m_client = new FlickrClient;
	m_loginDialog = NULL;
	
	connect(m_client, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
}

FlickrService::~FlickrService()
{
	delete m_session;
	delete m_client;
	delete m_loginDialog;
}

QString FlickrService::name() const
{
	QString name = m_session->fullName().isEmpty() ? m_session->userName() : m_session->fullName();
	return "Flickr (" + name + ")";
}

bool FlickrService::login()
{
#ifndef Q_WS_QWS
	QDesktopServices::openUrl(QUrl("http://www.flickr.com/auth-72157600238070065"));
#endif
	
	// For speed, we only create the dialog the first time it's used.  After that
	// we reuse it.
	if (!m_loginDialog)
	{
		m_loginDialog = new QDialog(NULL);
		m_loginUi = new Ui_FlickrLogin();
		m_loginUi->setupUi(m_loginDialog);
		m_loginUi->code->setValidator(new QRegExpValidator(QRegExp("\\d{3}-?\\d{3}-?\\d{3}"), m_loginUi->code));
#ifdef Q_WS_QWS
		m_loginUi->buttonBox->hide();
		QtopiaApplication::setInputMethodHint(m_loginUi->code, QtopiaApplication::Number);
#endif
	}
	
	// Clear the textbox
	m_loginUi->code->setText(QString::null);
	
	// Display the dialog
#ifdef Q_WS_QWS
	int returnCode = QtopiaApplication::execDialog(m_loginDialog);
#else
	int returnCode = m_loginDialog->exec();
#endif

	if (returnCode == QDialog::Rejected)
		return false;
	if (!m_loginUi->code->hasAcceptableInput())
		return false;
	
	QString miniToken = m_loginUi->code->text();
	
	// Invoke getFullToken on Flickr to convert our minitoken to
	// a full session which we can use to execute other operations.
	FlickrSession* session = m_client->getFullToken(miniToken);
	if (!session)
	{
		QMessageBox::warning(NULL, "Login error", m_client->errorMessage(), QMessageBox::Ok);
		return false;
	}
	
	// Store it
	m_session = session;
	
	return true;
}

QList<Album*> FlickrService::getAlbums()
{
	QList<Album*> ret = m_client->getPhotoSets(m_session);

	if (m_client->errorCode() != 0)
		QMessageBox::warning(NULL, "Get albums error", m_client->errorMessage(), QMessageBox::Ok);
	
	return ret;
}

qulonglong FlickrService::uploadPhoto(Album* album, const QImage& image, const QString& fileName)
{
	QString id = m_client->uploadPhoto(m_session, QFileInfo(fileName).baseName(), image);
	
	if (id == NULL)
		return 0;

	if (m_client->errorCode() != 0)
	{
		QMessageBox::warning(NULL, "Error uploading photo", m_client->errorMessage(), QMessageBox::Ok);
		return 0;
	}
	
	if (!album)
		return 0;
	
	// In Flickr, you can't just create an empty album.  When we create a new album
	// in greenphoto it exists purely on the client in a "Pending" state, and is
	// created here the first time a photo is uploaded to it.
	if (((FlickrPhotoSet*) album)->isPending())
		m_client->createPhotoSet(m_session, id, (FlickrPhotoSet*) album);
	else
		m_client->addPhotoToSet(m_session, id, (FlickrPhotoSet*) album);
	
	if (m_client->errorCode() != 0)
	{
		QMessageBox::warning(NULL, "Error adding photo to album", m_client->errorMessage(), QMessageBox::Ok);
		return 0;
	}
	
	return 0;
}

bool FlickrService::isError() const
{
	return m_client->errorCode() != 0;
}

bool FlickrService::requestAborted() const
{
	return m_client->requestAborted();
}

void FlickrService::abort()
{
	m_client->abort();
}

void FlickrService::save(QSettings* settings)
{
	if (!m_session)
		return;
	
	m_session->save(settings);
}

Album* FlickrService::createAlbum(const QString& name)
{
	return new FlickrPhotoSet(0, name, 0, true);
}

