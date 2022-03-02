#include <QApplication>
#include <QString>
#include "ui/pinger_main_window.hpp"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);
	PingerMainWindow pmw;

	pmw.show();

	return app.exec();

	// while (true) {
	// 	app.processEvents();
	// }

	// return 0;

}
