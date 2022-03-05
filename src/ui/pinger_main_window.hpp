#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QString>

#include <boost/asio.hpp>

#include <chrono>
#include <vector>
#include <string>


#include "ui_QtPingUI_1.h"


class PingerMainWindow : public QMainWindow, public Ui::MainWindow {

	Q_OBJECT;

public:
	PingerMainWindow(QWidget *parent = nullptr);
	~PingerMainWindow();	// delete all the widgets?
	

private:
	unsigned counter;
	boost::asio::io_context ui_ioc;
	boost::asio::steady_timer query_timer;


signals:
	void setSequenceN(const QString &text_seqn);
	
};


