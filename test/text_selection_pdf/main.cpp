#include <QActionGroup>
#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QPdfDocument>
#include <QToolBar>

#include "widgets/PdfView.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	const QString fileName = (argc > 1)
		? QString::fromLocal8Bit(argv[1])
		: QStringLiteral("test.pdf");

	QPdfDocument document;
	const QPdfDocument::Error err = document.load(fileName);
	if (err != QPdfDocument::Error::None)
	{
		qCritical() << "Failed to load PDF:" << fileName << "error:" << err;
		return 1;
	}

	QMainWindow window;
	auto *view = new PdfView(&window);
	view->setDocument(&document);
	view->setPageMode(QPdfView::PageMode::MultiPage);
	view->setMode(PdfView::Mode::SelectText);
	window.setCentralWidget(view);

	QToolBar *toolbar = window.addToolBar("Tools");
	auto *modeGroup = new QActionGroup(&window);
	modeGroup->setExclusive(true);

	QAction *handAct = toolbar->addAction("Hand");
	handAct->setCheckable(true);

	QAction *textAct = toolbar->addAction("Select Text");
	textAct->setCheckable(true);
	textAct->setChecked(true);

	QAction *imgAct = toolbar->addAction("Select Image");
	imgAct->setCheckable(true);

	modeGroup->addAction(handAct);
	modeGroup->addAction(textAct);
	modeGroup->addAction(imgAct);

	QObject::connect(
		handAct,
		&QAction::triggered,
		view,
		[view]() { view->setMode(PdfView::Mode::Hand); }
	);
	QObject::connect(
		textAct,
		&QAction::triggered,
		view,
		[view]() { view->setMode(PdfView::Mode::SelectText); }
	);
	QObject::connect(
		imgAct,
		&QAction::triggered,
		view,
		[view]() { view->setMode(PdfView::Mode::SelectImage); }
	);

	toolbar->addSeparator();
	QAction *copyAct = toolbar->addAction("Copy (Ctrl+C)");
	QObject::connect(
		copyAct,
		&QAction::triggered,
		view,
		&PdfView::copySelectionToClipboard
	);

	window.resize(900, 700);
	window.show();

	return app.exec();
}
