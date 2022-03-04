#pragma once

#include <QMainWindow>
#include <QWidget>

#include "ui_QtPingUI_1.h"


class PingerMainWindow : public QMainWindow, public Ui::MainWindow {

	Q_OBJECT;

public:
	PingerMainWindow(QWidget *parent = nullptr);
	~PingerMainWindow();	// delete all the widgets?

private:

public slots:
	
};


