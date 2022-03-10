#pragma once

#include <QWidget>
#include <QString>
#include <QListWidgetItem>

#include <vector>
#include <list>
#include <string>

#include <pinger.hpp>


//
// class HostListItem
// 

class HostListItem : public  {

	Q_OBJECT

public:

private:
	unsigned m_hostIndex;
	std::list<reply_status> m_replies;

};
