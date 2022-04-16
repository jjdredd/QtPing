#pragma once

#include <QWidget>
#include <QString>
#include <QListWidgetItem>

#include "pinger.hpp"


//
// class HostListItem
// 

// use
// https://doc.qt.io/qt-5/properties.html

class HostListItem : public QListWidgetItem  {

public:
	HostListItem() {}
	virtual ~HostListItem() {}

	unsigned GetKey() const { return m_hostKey; }
	void SetKey(unsigned k) { m_hostKey = k; }

private:
	unsigned m_hostKey;

};
