#include "tagselector.h"

TagSelector::TagSelector(QWidget* parent)
	: QDialog(parent),
	  m_sortedList(false)
{
	m_ui.setupUi(this);
#ifdef Q_WS_QWS
	m_ui.buttonBox->hide();
#endif

	connect(m_ui.friendList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), SLOT(currentItemChanged(QListWidgetItem*)));
	connect(m_ui.friendList, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(accept()));
	connect(m_ui.nameBox, SIGNAL(returnPressed()), SLOT(accept()));
}

TagSelector::~TagSelector()
{
}

void TagSelector::clearTaggables()
{
	m_ui.friendList->clear();
}

void TagSelector::addTaggable(Taggable taggable)
{
	QListWidgetItem* item = new QListWidgetItem(m_ui.friendList);
	item->setText(taggable.second);
	item->setData(Qt::UserRole, taggable.first);
}

QVariant TagSelector::selectedTaggable() const
{
	QListWidgetItem* item = m_ui.friendList->currentItem();
	if (item != NULL)
	{
		if (item->text() != m_ui.nameBox->text())
			return m_ui.nameBox->text();

		return item->data(Qt::UserRole);
	}
	else
		return m_ui.nameBox->text();
}

void TagSelector::currentItemChanged(QListWidgetItem* item)
{
	if (item == NULL)
		return;

	m_ui.nameBox->setText(item->text());
}

void TagSelector::showEvent(QShowEvent* /*event*/)
{
	if (m_sortedList)
		return;
	
	m_ui.friendList->sortItems();
	m_sortedList = true;
}
