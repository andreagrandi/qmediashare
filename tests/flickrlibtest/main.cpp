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

#include "flickrclient.h"
#include "flickrlibtest.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Greenphoto");
	QCoreApplication::setOrganizationDomain("invisiblesnowflake.com");
	QCoreApplication::setApplicationName("Greenphoto");

	FlickrClient::setApiKey("2c74b28d76190dc20eaeb6cef6edee68");
	FlickrClient::setSecret("6c98c7c9f5d3c656");
	
	FlickrlibTest* flibtest = new FlickrlibTest("612-270-444"); // Visit http://www.flickr.com/auth-72157600238070065 to get a mini token

	flibtest->hello();

	QString commentId1 = flibtest->addComment(1183071150, "commento di prova");
	QString commentId2 = flibtest->addComment(1183071150, "altro commento");

	QList<FlickrComment*> comments = flibtest->getPhotoComments("1183071150");

	flibtest->editComment(commentId1, "modifica commento");

	flibtest->deleteComment(commentId2);

	return app.exec();
}
