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
#include "facebookservice.h"
#include "facebookclient.h"
#include "facebooksession.h"
#include "facebookfriend.h"
#include "imagetagger.h"
#include "ui_facebooklogin.h"


#include <QInputDialog>
#include <QMessageBox>
#include <QDomElement>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QRegExpValidator>

#ifdef Q_WS_QWS
#include <QtopiaApplication>
#endif


FacebookService::FacebookService()
{
	m_session = NULL;
	m_client = new FacebookClient;
	m_loginDialog = NULL;
	
	connect(m_client, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
}

FacebookService::FacebookService(QSettings* settings)
{
	m_name = settings->value("name").toString();
	m_session = new FacebookSession(settings);
	m_client = new FacebookClient;
	m_loginDialog = NULL;
	
	connect(m_client, SIGNAL(uploadProgress(int, int)), SIGNAL(uploadProgress(int, int)));
}

FacebookService::~FacebookService()
{
	delete m_session;
	delete m_client;
	delete m_loginDialog;
}

bool FacebookService::login()
{
#ifndef Q_WS_QWS
	QDesktopServices::openUrl(QUrl("http://www.facebook.com/code_gen.php?v=1.0&api_key=" + FacebookClient::apiKey()));
#endif
	
	// For speed, we only create the dialog the first time it's used.  After that
	// we reuse it.
	if (!m_loginDialog)
	{
		m_loginDialog = new QDialog(NULL);
		m_loginUi = new Ui_FacebookLogin();
		m_loginUi->setupUi(m_loginDialog);
		m_loginUi->code->setValidator(new QRegExpValidator(QRegExp("\\w{6}"), m_loginUi->code)); // 6 letters or numbers
#ifdef Q_WS_QWS
		m_loginUi->buttonBox->hide();
		QtopiaApplication::setInputMethodHint(m_loginUi->code, QtopiaApplication::Text);
#endif
	}
	
	// Clear the textbox
	m_loginUi->code->setText(QString::null);
	
	// Show the dialog
#ifdef Q_WS_QWS
	int returnCode = QtopiaApplication::execDialog(m_loginDialog);
#else
	int returnCode = m_loginDialog->exec();
#endif

	if (returnCode == QDialog::Rejected)
		return false;
	if (!m_loginUi->code->hasAcceptableInput())
		return false;
	
	QString authToken = m_loginUi->code->text();
	
	// Invoke getSession on Facebook.  This turns our 6-character auth token
	// into a full session which we can use to execute queries.
	FacebookSession* session = m_client->getSession(authToken);
	if (!session)
	{
		QMessageBox::warning(NULL, "Login error", m_client->errorMessage(), QMessageBox::Ok);
		return false;
	}
	
	m_session = session;
	
	// Get the user's name.  This is a remote operation.
	m_name = m_client->getName(m_session);

	if (m_name.isEmpty())
	{
		QMessageBox::warning(NULL, "Login error", m_client->errorMessage(), QMessageBox::Ok);
		return false;
	}
	
	return true;
}

QList<Album*> FacebookService::getAlbums()
{
	QList<Album*> ret = m_client->getAlbums(m_session);

	if (m_client->errorCode() != 0)
		QMessageBox::warning(NULL, "Get albums error", m_client->errorMessage(), QMessageBox::Ok);
	
	return ret;
}

Album* FacebookService::createAlbum(const QString& name)
{
	Album* album = m_client->createAlbum(m_session, name);

	if (m_client->errorCode() != 0)
		QMessageBox::warning(NULL, "Error creating album", m_client->errorMessage(), QMessageBox::Ok);

	return album;
}

qulonglong FacebookService::uploadPhoto(Album* album, const QImage& image, const QString&)
{
	qulonglong ret = m_client->uploadPhoto(m_session, (FacebookAlbum*) album, image);

	if (m_client->errorCode() == 0)
		return ret;
	
	QMessageBox::warning(NULL, "Error uploading photo", m_client->errorMessage(), QMessageBox::Ok);
	return 0;
}

bool FacebookService::isError() const
{
	return m_client->errorCode() != 0;
}

bool FacebookService::requestAborted() const
{
	return m_client->requestAborted();
}

void FacebookService::abort()
{
	m_client->abort();
}

void FacebookService::save(QSettings* settings)
{
	settings->setValue("name", m_name);
	
	if (!m_session)
		return;
	
	m_session->save(settings);
}

void FacebookService::prepareForTagging()
{
	// This is called before tagPhoto.  Get a list of friends if we havn't
	// done so already
	if (m_friends.count() == 0)
		m_friends = m_client->getFriends(m_session);
}

void FacebookService::tagPhoto(qulonglong photoId, const QImage& image)
{
	ImageTagger* imageTagger = new ImageTagger(image);

	// Add the user, and all his friends
	imageTagger->addTaggable(Taggable(m_session->uid().toUInt(), "(Me)"));
	foreach (FacebookFriend f, m_friends)
		imageTagger->addTaggable(f.asTaggable());

	// Show the dialog
#ifdef Q_WS_QWS
	QtopiaApplication::execDialog(imageTagger);
#else
	imageTagger->exec();
#endif

	// Get the tags the user made
	QList<Tag> tags(imageTagger->tags());
	
	delete imageTagger;
	if (tags.count() == 0)
		return;

	// Turn each of the Tag objects into a FacebookTag object.
	// This is required because libgreenphoto can't depend on classes in this common
	// directory.
	QList<FacebookTag> facebookTags;
	foreach (Tag tag, tags)
		facebookTags << FacebookTag(QPoint(int(tag.first.x() * 100.0f), int(tag.first.y() * 100.0f)), tag.second);
	
	// Invoke addTags on facebook
	m_client->addTags(m_session, photoId, facebookTags);

	if (m_client->errorCode() != 0)
		QMessageBox::warning(NULL, "Error tagging photo", m_client->errorMessage(), QMessageBox::Ok);
}



