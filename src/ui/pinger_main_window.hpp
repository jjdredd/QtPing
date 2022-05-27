#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QSystemTrayIcon>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <chrono>
#include <vector>
#include <string>
#include <list>


#include "ui_QtPingUI_1.h"
#include "QtPingerCore.hpp"
#include "HostListItem.hpp"
#include "ping_charts.hpp"


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
//     QLineEdit *e_currentPing;
//     
//     QListWidget *HostListWidget;
//
//     QChartView *ChartView;
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

	void initTrayIcon();

	QtPingerCore m_appCore;
	QTimer m_timer;
	unsigned m_updateDelay;
	std::list<HostListItem *> m_listItems;
	PingLineChart m_lineChart;
	QSystemTrayIcon m_trayIcon;
	bool m_hiddenState;

	inline static const QString m_trayPic{":/tray_icon.png"};

public slots:

	// slot for timer
	void UpdateDisplay();

	// slot for ItemList
	void ItemSelected(QListWidgetItem *);

	// buttons
	void AddHost();
	void DeleteHost();

	// tray icon slots
	void trayClicked(QSystemTrayIcon::ActivationReason);

signals:

	void UpdateHostName(const QString &);
	void UpdateAddress(const QString &);
	void UpdateCurrent(const QString &);
	void UpdateAverage(const QString &);
	void UpdateStdDev(const QString &);
	void UpdateMin(const QString &);
	void UpdateMax(const QString &);
	void UpdateLost(const QString &);
	void UpdateTTL(const QString &);

	void ClearDisplayStats();
};
