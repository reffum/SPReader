//
// The application main window
//
#include "MainWindow.h"
#include "settings_keys.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QPdfDocument>
#include <QPdfView>
#include <QPdfPageNavigator>
#include <QMessageBox>
#include <QSpinBox>

#include "models/PdfContentsModel.h"

constexpr double CONTENTS_DOCKWIDGET_PART = 0.3;
constexpr double DEFAULT_ZOOM_FACTOR = 1.0;
constexpr double MAXIMUM_ZOOM_FACTOR = 5.0;
constexpr double ZOOM_FACTOR_STEP = 0.25;

MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent),
	m_pdfDocument(new QPdfDocument(this))
{
	setupUi(this);

	auto pdfView = dynamic_cast<QPdfView *>(centralWidget());
	pdfView->setDocument(m_pdfDocument);
	pdfView->setPageMode(QPdfView::PageMode::MultiPage);

	int contents_dockWidgetSize = static_cast<int>(width() * CONTENTS_DOCKWIDGET_PART);
	resizeDocks(
		{ contents_dockWidget } ,
		{ contents_dockWidgetSize } ,
		Qt::Horizontal
		);

	m_pageSpinBox = new QSpinBox(this);
	toolBar->insertWidget(actionNext_page, m_pageSpinBox);

	m_zoomSpinBox = new QDoubleSpinBox(this);
	toolBar->insertWidget(actionZoom_Out, m_zoomSpinBox);
	m_zoomSpinBox->setMaximum(MAXIMUM_ZOOM_FACTOR * 100);
	m_zoomSpinBox->setMinimum(ZOOM_FACTOR_STEP * 100.0);
	m_zoomSpinBox->setValue(DEFAULT_ZOOM_FACTOR * 100);
	m_zoomSpinBox->setSingleStep(ZOOM_FACTOR_STEP * 100);

	connect(
		m_pageSpinBox,
		SIGNAL(valueChanged(int)),
		this,
		SLOT(on_m_pageSpinBox_valueChanged(int))
	);

	connect(
		pdfView->pageNavigator(),
		SIGNAL(currentPageChanged(int)),
		this,
		SLOT(on_pageNavigator_currentPageChanged(int))
	);

	connect(
		m_zoomSpinBox,
		SIGNAL(valueChanged(double)),
		this,
		SLOT(on_m_zoomSpinBox_valueChanged(double))
	);
}

void MainWindow::on_action_Open_triggered(bool checked)
{
	QSettings settings;
	QString lastDir = settings.value(Settings::LastDir, QDir::homePath()).toString();

	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Open PDF File"),
		lastDir,
		tr("PDF Files (*.pdf)")
	);
	if (!fileName.isEmpty()) {
		QFileInfo checkFile(fileName);
		if (checkFile.exists() && checkFile.isFile()) {
			qInfo() << "Open existing PDF file:" << fileName;
			settings.setValue(Settings::LastDir, checkFile.absolutePath());
		} else {
			qWarning() << "File does not exist:" << fileName;
		}

		QPdfDocument::Error r = m_pdfDocument->load(fileName);
		if (r == QPdfDocument::Error::None)
		{
			qInfo() << "PDF file loaded:" << fileName;
		}
		else
		{
			qCritical() << "PDF file load failed:" << r;
			QMessageBox::critical(
				this,
				tr("Error"),
				tr("File open error")
			);
		}

		// Load the contents model in contents tree view
		auto * contentsModel = new PdfContentsModel(this);
		contentsModel->setDocument(m_pdfDocument);
		contents_treeView->setModel(contentsModel);

		int pageCount = m_pdfDocument->pageCount();
		m_pageSpinBox->setMaximum(pageCount);
	}
}

void MainWindow::on_contents_treeView_navigateToPage(int pageNumber) const
{
	QSignalBlocker blocker(m_pageSpinBox);

	auto pdfView = dynamic_cast<QPdfView *>(centralWidget());
	pdfView->pageNavigator()->jump(pageNumber, {});

	m_pageSpinBox->setValue(pageNumber);
}

void MainWindow::on_actionNext_page_triggered(bool checked) const
{
	QSignalBlocker blocker(m_pageSpinBox);

	auto pdfView = dynamic_cast<QPdfView *>(centralWidget());

	QPdfPageNavigator * navigator = pdfView->pageNavigator();
	int nextPage = navigator->currentPage() + 1;
	navigator->jump(nextPage, {});
	m_pageSpinBox->setValue(nextPage);
}

void MainWindow::on_actionPrev_page_triggered(bool checked) const
{
	QSignalBlocker blocker(m_pageSpinBox);

	auto pdfView = dynamic_cast<QPdfView *>(centralWidget());

	QPdfPageNavigator * navigator = pdfView->pageNavigator();
	int prevPage = navigator->currentPage() - 1;
	if (prevPage >= 0)
	{
		navigator->jump(prevPage, {});
		m_pageSpinBox->setValue(prevPage);
	}
}

void MainWindow::on_m_pageSpinBox_valueChanged(int value) const
{
	auto pdfView = dynamic_cast<QPdfView *>(centralWidget());
	QPdfPageNavigator * navigator = pdfView->pageNavigator();
	navigator->jump(value, {});
}

void MainWindow::pageNavigator_currentPageChanged(int value) const
{
	QSignalBlocker blocker(m_pageSpinBox);
	m_pageSpinBox->setValue(value);
}

void MainWindow::on_actionZoom_In_triggered(bool checked) const
{
	QSignalBlocker blocker(m_zoomSpinBox);

	auto pdfView = dynamic_cast<QPdfView *>(centralWidget());
	double zoomFactor = pdfView->zoomFactor();
	zoomFactor += ZOOM_FACTOR_STEP;

	pdfView->setZoomFactor(zoomFactor);
	m_zoomSpinBox->setValue(zoomFactor * 100);
}

void MainWindow::on_actionZoom_Out_triggered(bool checked) const
{
	QSignalBlocker blocker(m_zoomSpinBox);

	auto pdfView = dynamic_cast<QPdfView *>(centralWidget());
	double zoomFactor = pdfView->zoomFactor();
	if (zoomFactor - ZOOM_FACTOR_STEP > 0.0)
	{
		zoomFactor -= ZOOM_FACTOR_STEP;
		pdfView->setZoomFactor(zoomFactor);
		m_zoomSpinBox->setValue(zoomFactor * 100);
	}
}

void MainWindow::on_m_zoomSpinBox_valueChanged(double) const
{
	double zoomFactor = m_zoomSpinBox->value() / 100.0;
	auto pdfView = dynamic_cast<QPdfView *>(centralWidget());
	pdfView->setZoomFactor(zoomFactor);
}
