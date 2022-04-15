#pragma once

#include <QWidget>
#include <QString>
#include <QListWidgetItem>

#include "pinger.hpp"


//
// class HostListItem
// 

class HostListItem : public QListWidgetItem  {

	Q_OBJECT;

public:
	HostListItem() {}
	virtual ~HostListItem() {}

	unsigned GetKey() const { return m_hostKey; }
	void SetKey(unsigned k) { m_hostKey = k; }

private:
	unsigned m_hostKey;

};
