#ifndef FACEBOOKFRIEND_H
#define FACEBOOKFRIEND_H

#include <QString>

typedef QPair<uint, QString> Taggable;

class FacebookFriend
{
public:
	FacebookFriend() {}
	~FacebookFriend() {}
	
	void setId(uint id) { m_id = id; }
	void setName(const QString& name) { m_name = name; }
	
	uint id() const { return m_id; }
	QString name() const { return m_name; }

	Taggable asTaggable() const { return Taggable(m_id, m_name); }
	
private:
	QString m_name;
	uint m_id;
};

#endif

