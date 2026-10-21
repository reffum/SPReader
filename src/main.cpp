#include "constants.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
	QCoreApplication::setApplicationName(APPLICATION_NAME);

	MainWindow mainWindow;

	// Open documents passed in the command line
	QStringList arguments = QApplication::arguments();

	arguments.removeFirst();

	for (QString& fileName : arguments)
	{
		qInfo() << "Open file: " << fileName;
		mainWindow.openFile(fileName);
	}

	mainWindow.show();

	return QApplication::exec();
}
