#ifndef TAGSELECTOR_H
#define TAGSELECTOR_H

#include "ui_tagselector.h"

typedef QPair<uint, QString> Taggable;

#include <QDialog>
#include <QShowEvent>

class TagSelector : public QDialog
{
	Q_OBJECT

public:
	TagSelector(QWidget* parent = NULL);
	~TagSelector();

	void clearTaggables();
	void addTaggable(Taggable taggable);
	QVariant selectedTaggable() const;

	void showEvent(QShowEvent* event);

private slots:
	void currentItemChanged(QListWidgetItem* item);

private:
	Ui_TagSelector m_ui;
	bool m_sortedList;
};

#endif
