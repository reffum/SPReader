#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName(
		"ReffumComp"
	);
	QCoreApplication::setApplicationName(
		"SPReader"
	);

	MainWindow mainWindow;
	mainWindow.show();

	return QApplication::exec();
}
