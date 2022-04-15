#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QThread>
#include <QTimer>

#include <boost/asio.hpp>

#include <chrono>
#include <vector>
#include <string>
#include <list>


#include "ui_QtPingUI_1.h"
#include "QtPingerCore.hpp"
#include "HostListItem.hpp"


//
// class PingerMainWindow
// 

// the widgets are
//     QPushButton *b_AddHost;
//     QPushButton *b_DeleteHost;
//     
//     QLabel *l_HostName;
//     QLabel *l_Address;
//     QLabel *l_PingHint;
//     QLabel *l_Average;
//     QLabel *l_StdDev;
//     QLabel *l_Min;
//     QLabel *l_Max;
//     QLabel *l_Lost;
//     QLabel *l_TTL;
//     
//     QLineEdit *e_HostName;
//     QLineEdit *e_Address;
//     QLineEdit *e_Average;
//     QLineEdit *e_StdDev;
//     QLineEdit *e_Min;
//     QLineEdit *e_Max;
//     QLineEdit *e_lost;
//     QLineEdit *e_TTL;
//     
//     QListWidget *HostListWidget;
//     
//     QFrame *framePlot;
//     
//     QWidget *centralwidget;
//     
//     QMenuBar *Menu;
//     QStatusBar *Status;


class PingerMainWindow : public QMainWindow, public Ui::MainWindow {

	Q_OBJECT;

public:
	PingerMainWindow(QWidget *parent = nullptr);
	virtual ~PingerMainWindow();	// delete all the widgets?

private:
	QtPingerCore m_appCore;
	QTimer m_timer;
	unsigned m_updateDelay;
	std::list<HostListItem *> m_listItems;

public slots:

	// slot for timer
	void UpdateDisplay();

	// slot for ItemList
	void ItemSelected(QListWidgetItem *);

	// buttons
	void AddHost();
	void DeleteHost();

signals:

	void UpdateHostName(const QString &);
	void UpdateAddress(const QString &);
	void UpdateAverage(const QString &);
	void UpdateStdDev(const QString &);
	void UpdateMin(const QString &);
	void UpdateMax(const QString &);
	void UpdateLost(const QString &);
	void UpdateTTL(const QString &);

	void ClearDisplayStats();
};
