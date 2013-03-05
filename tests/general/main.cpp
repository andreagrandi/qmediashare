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

#include <QApplication>

#include "test.h"
#include "facebookclient.h"
#include "flickrclient.h"
#include "gpicasaclient.h"

int main(int argc, char** argv)
{
	qsrand(time(0));
	
	QApplication app(argc, argv);
	
	QCoreApplication::setOrganizationName("Greenphoto");
	QCoreApplication::setOrganizationDomain("invisiblesnowflake.com");
	QCoreApplication::setApplicationName("Greenphoto");

	FacebookClient::setApiKey("d213e08f0ebb50b0943b91e1658af877");
	FacebookClient::setSecret("95cc1e936541bd8768b5bbcdb184a545");

	FlickrClient::setApiKey("2c74b28d76190dc20eaeb6cef6edee68");
	FlickrClient::setSecret("6c98c7c9f5d3c656");
	
#ifdef HAS_GDATA
	GPicasaClient::setVersion("0.1");
#endif
	
	Test* test = new Test(NULL);
	test->show();

	return app.exec();
}
