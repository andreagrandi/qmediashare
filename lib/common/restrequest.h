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

#ifndef RESTREQUEST_H
#define RESTREQUEST_H

#include <QUrl>
#include <QMap>
#include <QHttpResponseHeader>

class QHttp;

// Abstract base class for RESTful services
// Derived classes should implement parseResponse and errorResponse to deal
// with service-specific responses.
class RestRequest : public QObject
{
	Q_OBJECT
public:
	RestRequest();
	RestRequest(const QUrl& endpoint);
	virtual ~RestRequest() {}
	
	void setEndpoint(const QUrl& endpoint);

	int errorCode() const { return m_errorCode; }
	QString errorMessage() const { return m_errorMessage; }
	bool aborted() const { return m_aborted; }

	void setArgument(const QString& name, const QString& value);
	void setArgument(const QString& name, int value);
	void setArgument(const QString& name, uint value);
	void setArgument(const QString& name, qulonglong value);
	void setHeader(const QString& name, const QString& value);
	void setMimeData(const QString& filename, const QString& mimetype, const QByteArray& data, const QString& name = QString::null);
	virtual void start();
	void abort();

protected:
	virtual void parseResponse(const QString& data) = 0;
	virtual void errorResponse() = 0;
	virtual QTcpSocket* customSocket() { return NULL; }
	
	int m_errorCode;
	QString m_errorMessage;
	bool m_aborted;

	QMap<QString, QString> m_attributes;
	QMap<QString, QString> m_headers;
	QString m_mimeFilename;
	QString m_mimeType;
	QByteArray m_mimeData;
	QString m_mimeName;
	
	bool m_redirecting;
	QHttp* m_http;
	QUrl m_endpoint;

signals:
	void uploadProgress(int bytes, int totalBytes);
	void complete(bool error);

private slots:
	void requestFinished(int id, bool error);
	void responseHeaderReceived(const QHttpResponseHeader& resp);

private:
	QString boundaryString() const;
	static QString escapePost(const QString& input);
};

#endif
