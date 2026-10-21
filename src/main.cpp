#include "constants.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
	QCoreApplication::setApplicationName(APPLICATION_NAME);

	MainWindow mainWindow;
	mainWindow.show();

	return QApplication::exec();
}
