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

#include "imagetagger.h"
#include "tagselector.h"

#include <QPaintEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPen>
#ifdef Q_WS_QWS
#include <QtopiaApplication>
#include <QSoftMenuBar>
#endif

#define RECT_SIZE QSize(70, 70)

TagSelector* ImageTagger::m_tagSelector = NULL;

ImageTagger::ImageTagger(const QImage& image)
	: m_originalImage(image),
	  m_point(0.5f, 0.5f),
	  m_scaledImage(image),
	  m_scaleFactor(1.0),
	  m_imageTopLeft(0, 0)
{
	if (m_tagSelector != NULL)
		m_tagSelector->clearTaggables();

#ifdef Q_WS_QWS
	// Set up the menus for the phone's context button
	QMenu* menu = QSoftMenuBar::menuFor(this);

	m_removeMenu = new QMenu("Remove tag", this);
	m_removeMenu->setEnabled(false);
	menu->addMenu(m_removeMenu);

	m_removeAllAction = menu->addAction("Remove all tags", this, SLOT(removeAllTags()));
	m_removeAllAction->setEnabled(false);

	QSoftMenuBar::setLabel(this, Qt::Key_Select, QSoftMenuBar::Select);
#endif
}

ImageTagger::~ImageTagger()
{
}

void ImageTagger::addTaggable(Taggable taggable)
{
	m_taggables[taggable.first] = taggable.second;
	tagSelector()->addTaggable(taggable);
}

void ImageTagger::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	
	// Draw the image
	p.drawImage(m_imageTopLeft, m_scaledImage);
	
	// Draw any existing tag boxes
	QPen pen(Qt::gray);
	pen.setWidth(2);
	p.setPen(pen);
	foreach (Tag tag, m_tags)
		p.drawRect(calculateFaceRect(tag.first));

	// Draw any existing tag names
	QFont font(QApplication::font());
	font.setPixelSize(10);
	p.setFont(font);
	QFontMetrics metrics(font);
	foreach (Tag tag, m_tags)
	{
		QString name(tagName(tag.second));
		QPoint point(scalePoint(tag.first));

		QRect textRect(metrics.boundingRect(name));
		textRect.setWidth(textRect.width() + 6);
		textRect.moveLeft(qBound(0, point.x() - textRect.width()/2, m_scaledImage.width()));
		textRect.moveTop(qBound(0, point.y() + RECT_SIZE.height()/2 + 4, m_scaledImage.height() - textRect.height()));
		textRect.translate(m_imageTopLeft);

		p.setPen(Qt::white);
		p.setBrush(Qt::white);
		p.drawRect(textRect);
		
		p.setPen(Qt::black);
		p.drawText(textRect, Qt::AlignCenter, name);
	}
	
	// Draw the face rect
	pen.setColor(Qt::blue);
	p.setPen(pen);
	p.setBrush(Qt::NoBrush);
	p.drawRect(calculateFaceRect(m_point));
}

QString ImageTagger::tagName(QVariant tag) const
{
	if (tag.type() == QVariant::String)
		return tag.toString();
	else
		return m_taggables[tag.toUInt()];
}

QRect ImageTagger::calculateFaceRect(const QPointF& point)
{
	QRect rect(scalePoint(point), RECT_SIZE);
	rect.translate(-RECT_SIZE.width()/2, -RECT_SIZE.height()/2);
	rect.translate(m_imageTopLeft);
	return rect;
}

QPoint ImageTagger::scalePoint(const QPointF& point)
{
	return QPoint(int(point.x()*m_scaledImage.width()),
		int(point.y()*m_scaledImage.height()));
}

void ImageTagger::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
		case Qt::Key_Left: moveFaceRect(-1, 0); break;
		case Qt::Key_Right: moveFaceRect(1, 0); break;
		case Qt::Key_Up: moveFaceRect(0, -1); break;
		case Qt::Key_Down: moveFaceRect(0, 1); break;
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Select:
			newTag(); break;
		default:
			e->ignore();
			return;
	}
	e->accept();
}

void ImageTagger::mouseReleaseEvent(QMouseEvent* e)
{
	setFaceRectPos(e->pos());
	newTag();
}

void ImageTagger::mouseMoveEvent(QMouseEvent* e)
{
	setFaceRectPos(e->pos());
}

void ImageTagger::setFaceRectPos(const QPoint& point)
{
	QPoint p(point - m_imageTopLeft);
	m_point = QPointF(float(p.x()) / float(m_scaledImage.width()),
	                  float(p.y()) / float(m_scaledImage.height()));
	ensureFaceRectValid();
	update();
}

void ImageTagger::moveFaceRect(int dx, int dy)
{
	QPointF delta(dx, dy);
	delta /= 10.0f;
	m_point += delta;
	
	ensureFaceRectValid();
	update();
}

void ImageTagger::ensureFaceRectValid()
{
	m_point.setX(qBound(qreal(0.0), m_point.x(), qreal(1.0)));
	m_point.setY(qBound(qreal(0.0), m_point.y(), qreal(1.0)));
}

void ImageTagger::resizeEvent(QResizeEvent*)
{
	m_scaledImage = m_originalImage.scaled(size(), Qt::KeepAspectRatio);
	m_scaleFactor = double(m_scaledImage.width()) / double(m_originalImage.width());
	
	m_imageTopLeft = QPoint((width() - m_scaledImage.width()) / 2,
	                        (height() - m_scaledImage.height()) / 2);
}

void ImageTagger::newTag()
{
#ifdef Q_WS_QWS
	int ret = QtopiaApplication::execDialog(tagSelector());
#else
	int ret = tagSelector()->exec();
#endif

	if (ret == QDialog::Rejected)
		return;

	QVariant tag(m_tagSelector->selectedTaggable());

	m_removeActions[m_removeMenu->addAction(tagName(tag), this, SLOT(removeTag()))] = tag;
	m_removeAllAction->setEnabled(true);
	m_removeMenu->setEnabled(true);
	
	m_tags << Tag(m_point, tag);
	update();
}

QList<Tag> ImageTagger::tags()
{
	return m_tags;
}

TagSelector* ImageTagger::tagSelector()
{
	if (m_tagSelector == NULL)
		m_tagSelector = new TagSelector(this);
	return m_tagSelector;
}

void ImageTagger::removeAllTags()
{
	m_removeMenu->clear();
	m_removeMenu->setEnabled(false);
	m_removeActions.clear();
	m_removeAllAction->setEnabled(false);
	
	m_tags.clear();
	update();
}

void ImageTagger::removeTag()
{
	QAction* action = (QAction*) sender();
	QVariant tag(m_removeActions[action]);

	// Remove the item from tags
	QList<Tag>::iterator it;
	for (it=m_tags.begin() ; it!=m_tags.end() ; ++it)
	{
		if ((*it).second == tag)
		{
			m_tags.erase(it);
			break;
		}
	}

	m_removeActions.remove(action);
	delete action;
	m_removeAllAction->setEnabled(!m_removeActions.isEmpty());
	m_removeMenu->setEnabled(!m_removeActions.isEmpty());
	update();
}

