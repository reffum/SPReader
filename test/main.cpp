#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>

#include "data/DjvuDocument.h"
#include "widgets/DjvuView.h"

int main(int argc, char * argv[])
{
	QApplication app(argc, argv);

	QCommandLineParser parser;
	parser.setApplicationDescription(
		"Test app that creates a DjvuView"
	);
	parser.addHelpOption();
	parser.addPositionalArgument(
		"file",
		"Optional DjVu file to load"
	);
	parser.process(app);

	DjvuDocument document;

	const bool r = document.load("Основы_искусственного_интеллекта_нетехническое_введение_2021_Том.djvu");
	assert(r);

	DjvuView view;
	view.setDocument(&document);
	view.setZoomFactor(1.0);
	view.resize(800, 600);
	view.show();

	const QStringList args = parser.positionalArguments();
	if (!args.isEmpty())
	{
		if (document.load(args.first()))
		{
			qInfo() << "Loaded" << document.pageCount() << "page(s)";
		}
		else
		{
			qWarning() << "Failed to load:" << args.first();
		}
	}

	return QApplication::exec();
}