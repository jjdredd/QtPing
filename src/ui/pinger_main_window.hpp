#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QThread>

#include <boost/asio.hpp>

#include <chrono>
#include <vector>
#include <string>


#include "ui_QtPingUI_1.h"


class PingerThread : public QThread {
	Q_OBJECT

public:
	PingerThread();
	~PingerThread();

	void StopThread();

private:
	void run() override;

	unsigned m_counter;
	boost::asio::io_context m_ui_ioc;
	boost::asio::steady_timer m_query_timer;

	QString text_TTL;

signals:
	void setSequenceN(const QString &text_seqn);

};


//
// class PingerMainWindow
// 

class PingerMainWindow : public QMainWindow, public Ui::MainWindow {

	Q_OBJECT;

public:
	PingerMainWindow(QWidget *parent = nullptr);
	~PingerMainWindow();	// delete all the widgets?

private:
	PingerThread m_pt;

public slots:
	void printTextTest();


signals:
		
};
