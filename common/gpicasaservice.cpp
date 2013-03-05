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

// GData uses HTTPS which is not available before Qt 4.3
#include <QtGlobal>
#if QT_VERSION >= 0x040300

#include <QSettings>
#include <QDialog>
#include <QMessageBox>

#include "gpicasaservice.h"
#include "gpicasaclient.h"
#include "gdatasession.h"

GPicasaService::GPicasaService()
{
	init();
}

GPicasaService::GPicasaService(QSettings* settings)
{
	init();
	m_session = new GDataSession(settings);
}

void GPicasaService::init()
{
	m_session = NULL;
	m_client = new GPicasaClient;
	m_loginDialog = NULL;
}

GPicasaService::~GPicasaService()
{
	delete m_session;
	delete m_client;
	delete m_loginDialog;
}

bool GPicasaService::login()
{
	if (!m_loginDialog)
	{
		m_loginDialog = new QDialog();
		m_loginDialogUi.setupUi(m_loginDialog);
	}
	
	m_loginDialogUi.emailBox->setText("");
	m_loginDialogUi.passwordBox->setText("");
	
	if (m_loginDialog->exec() != QDialog::Accepted)
		return false;
	
	QString email = m_loginDialogUi.emailBox->text();
	QString password = m_loginDialogUi.passwordBox->text();
	
	GDataSession* session = m_client->login(email, password);
	if (!session)
	{
		QMessageBox::warning(NULL, "Login error", m_client->errorMessage(), QMessageBox::Ok);
		return false;
	}
	
	m_session = session;
	
	return true;
}

bool GPicasaService::isError() const
{
	return m_client->errorCode() != 0;
}

void GPicasaService::save(QSettings* settings)
{
	if (!m_session)
		return;
	
	m_session->save(settings);
}

#endif

