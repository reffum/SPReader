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
#include <QIcon>

#include "models/PdfContentsModel.h"

constexpr double CONTENTS_DOCKWIDGET_PART = 0.3;
constexpr double DEFAULT_ZOOM_FACTOR = 1.0;
constexpr double MAXIMUM_ZOOM_FACTOR = 5.0;
constexpr double ZOOM_FACTOR_STEP = 0.25;

MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent)
{
	setupUi(this);

	setWindowIcon(
		QIcon(":/resources/icon.svg")
	);

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
		SLOT(m_pageSpinBox_valueChanged(int))
	);

	connect(
		m_zoomSpinBox,
		SIGNAL(valueChanged(double)),
		this,
		SLOT(m_zoomSpinBox_valueChanged(double))
	);

	centralwidget->setElideMode(Qt::ElideRight);

	centralwidget->setStyleSheet(
		"QTabBar::tab { max-width: " +
		QString::number(centralwidget->width() / 3) +
		"px; }"
	);
}

QPdfView * MainWindow::currentPdfView() const
{
	auto currentWidget = centralwidget->currentWidget();
	if (currentWidget == nullptr)
	{
		return nullptr;
	}
	return dynamic_cast<QPdfView *>(currentWidget);
}

QPdfDocument * MainWindow::currentPdfDocument() const
{
	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return nullptr;
	}
	return pdfView->document();
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(
		this,
		tr("About SPReader"),
		tr("SPReader\n"
		   "PDF/DJVU/FB file reader.\n\n"
		   "Version: ") +
		QString(PROJECT_VERSION)
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

		auto * pdfDocument = new QPdfDocument(this);
		QPdfDocument::Error r = pdfDocument->load(fileName);
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
			delete pdfDocument;
			return;
		}

		// ReSharper disable once CppDFAMemoryLeak
		auto * pdfView = new QPdfView(this);
		pdfView->setDocument(pdfDocument);
		pdfView->setPageMode(QPdfView::PageMode::MultiPage);

		connect(
			pdfView->pageNavigator(),
			SIGNAL(currentPageChanged(int)),
			this,
			SLOT(pageNavigator_currentPageChanged(int))
		);

		int tabIndex = centralwidget->addTab(
			pdfView,
			checkFile.fileName()
		);
		centralwidget->setTabToolTip(
			tabIndex,
			checkFile.fileName()
		);
		centralwidget->setCurrentIndex(tabIndex);
	}
}

void MainWindow::on_centralwidget_currentChanged(int index)
{
	QSignalBlocker blocker0(m_pageSpinBox);
	QSignalBlocker blocker1(m_zoomSpinBox);

	QPdfView * pdfView = currentPdfView();
	QPdfDocument * pdfDocument = currentPdfDocument();

	if (pdfView == nullptr || pdfDocument == nullptr)
	{
		contents_treeView->setModel(nullptr);
		m_pageSpinBox->setMaximum(0);
		m_pageSpinBox->setValue(0);
		return;
	}

	// Load the contents model in contents tree view
	auto * contentsModel = new PdfContentsModel(this);
	contentsModel->setDocument(pdfDocument);
	contents_treeView->setModel(contentsModel);

	int pageCount = pdfDocument->pageCount();
	m_pageSpinBox->setMaximum(pageCount);

	int currentPage = pdfView->pageNavigator()->currentPage();
	m_pageSpinBox->setValue(currentPage);
	m_zoomSpinBox->setValue(pdfView->zoomFactor() * 100);
}

void MainWindow::on_centralwidget_tabCloseRequested(int index) const
{
	QWidget * widget = centralwidget->widget(index);
	if (widget == nullptr)
	{
		return;
	}

	auto * pdfView = dynamic_cast<QPdfView *>(widget);
	if (pdfView != nullptr)
	{
		QPdfDocument * pdfDocument = pdfView->document();
		if (pdfDocument != nullptr)
		{
			delete pdfDocument;
		}
	}

	centralwidget->removeTab(index);
	delete widget;
}

void MainWindow::on_contents_treeView_navigateToPage(int pageNumber) const
{
	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

	QSignalBlocker blocker(m_pageSpinBox);

	pdfView->pageNavigator()->jump(pageNumber, {});

	m_pageSpinBox->setValue(pageNumber);
}

void MainWindow::on_actionNext_page_triggered(bool checked) const
{
	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

	QSignalBlocker blocker(m_pageSpinBox);

	QPdfPageNavigator * navigator = pdfView->pageNavigator();
	int nextPage = navigator->currentPage() + 1;
	navigator->jump(nextPage, {});
	m_pageSpinBox->setValue(nextPage);
}

void MainWindow::on_actionPrev_page_triggered(bool checked) const
{
	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

	QSignalBlocker blocker(m_pageSpinBox);

	QPdfPageNavigator * navigator = pdfView->pageNavigator();
	int prevPage = navigator->currentPage() - 1;
	if (prevPage >= 0)
	{
		navigator->jump(prevPage, {});
		m_pageSpinBox->setValue(prevPage);
	}
}

void MainWindow::m_pageSpinBox_valueChanged(int value) const
{
	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}
	QPdfPageNavigator * navigator = pdfView->pageNavigator();
	navigator->jump(value, {});
}

void MainWindow::pageNavigator_currentPageChanged(int value) const
{
	// Only update page spinbox if the active tab triggered the signal
	auto * senderNavigator = dynamic_cast<QPdfPageNavigator *>(sender());
	QPdfView * pdfView = currentPdfView();
	if (pdfView != nullptr && senderNavigator == pdfView->pageNavigator())
	{
		QSignalBlocker blocker(m_pageSpinBox);
		m_pageSpinBox->setValue(value);
	}
}

void MainWindow::on_actionZoom_In_triggered(bool checked) const
{
	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

	QSignalBlocker blocker(m_zoomSpinBox);

	double zoomFactor = pdfView->zoomFactor();
	zoomFactor += ZOOM_FACTOR_STEP;

	pdfView->setZoomFactor(zoomFactor);
	m_zoomSpinBox->setValue(zoomFactor * 100);
}

void MainWindow::on_actionZoom_Out_triggered(bool checked) const
{
	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

	QSignalBlocker blocker(m_zoomSpinBox);

	double zoomFactor = pdfView->zoomFactor();
	if (zoomFactor - ZOOM_FACTOR_STEP > 0.0)
	{
		zoomFactor -= ZOOM_FACTOR_STEP;
		pdfView->setZoomFactor(zoomFactor);
		m_zoomSpinBox->setValue(zoomFactor * 100);
	}
}

void MainWindow::m_zoomSpinBox_valueChanged(double) const
{
	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}
	double zoomFactor = m_zoomSpinBox->value() / 100.0;
	pdfView->setZoomFactor(zoomFactor);
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
	QMainWindow::resizeEvent(event);
	centralwidget->setStyleSheet(
		"QTabBar::tab { max-width: " +
		QString::number(centralwidget->width() / 4) +
		"px; }"
	);
}
