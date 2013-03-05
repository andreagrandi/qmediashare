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

#ifndef FACEBOOKTAGGER_H
#define FACEBOOKTAGGER_H

#include <QImage>
#include <QDialog>
#include <QPoint>
#include <QMap>
#include <QMenu>

typedef QPair<uint, QString> Taggable;
typedef QPair<QPointF, QVariant> Tag;

class TagSelector;

class QPaintEvent;
class QKeyEvent;

class ImageTagger : public QDialog
{
	Q_OBJECT

public:
	ImageTagger(const QImage& image);
	~ImageTagger();
	
	void addTaggable(Taggable taggable);
	QList<Tag> tags();

	QSize sizeHint() const { return QSize(240, 320); }

private slots:
	void removeAllTags();
	void removeTag();

private:
	void paintEvent(QPaintEvent* e);
	void keyPressEvent(QKeyEvent* e);
	void resizeEvent(QResizeEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	
	void setFaceRectPos(const QPoint& delta);
	void moveFaceRect(int dx, int dy);
	void ensureFaceRectValid();
	QRect calculateFaceRect(const QPointF& point);
	QPoint scalePoint(const QPointF& point);
	QString tagName(QVariant tag) const;
	
	void newTag();
	TagSelector* tagSelector();
	
	const QImage m_originalImage;
	QPointF m_point;
	
	QImage m_scaledImage;
	double m_scaleFactor;
	QPoint m_imageTopLeft;
	
	QMap<uint, QString> m_taggables;
	QList<Tag> m_tags;
	QMap<QAction*, QVariant> m_removeActions;

	static TagSelector* m_tagSelector;

	QMenu* m_removeMenu;
	QAction* m_removeAllAction;
};

#endif

