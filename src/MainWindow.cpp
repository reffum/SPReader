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
#include "data/DjvuDocument.h"
#include "models/DjvuContentsModel.h"
#include "widgets/DjvuView.h"

constexpr double CONTENTS_DOCKWIDGET_PART = 0.3;
constexpr double DEFAULT_ZOOM_FACTOR = 1.0;
constexpr double MAXIMUM_ZOOM_FACTOR = 5.0;
constexpr double ZOOM_FACTOR_STEP = 0.25;
constexpr int MAX_RECENT_FILES = 10;

MainWindow::MainWindow(
	QWidget * parent
):
	QMainWindow(parent)
{
	setupUi(this);

	setWindowIcon(
		QIcon(":/resources/icon.svg")
	);

	updateRecentFilesMenu();

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

DjvuView * MainWindow::currentDjvuView() const
{
	auto currentWidget = centralwidget->currentWidget();
	if (currentWidget == nullptr)
	{
		return nullptr;
	}
	return dynamic_cast<DjvuView *>(currentWidget);
}

DjvuDocument * MainWindow::currentDjvuDocument() const
{
	DjvuView * djvuView = currentDjvuView();
	if (djvuView == nullptr)
	{
		return nullptr;
	}
	return djvuView->document();
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

void MainWindow::on_action_Open_triggered(
	bool checked
)
{
	QSettings settings;
	QString lastDir = settings.value(
		Settings::LastDir,
		QDir::homePath()
	).toString();

	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Open Document"),
		lastDir,
		tr("Supported Files (*.pdf *.djvu *.djv);;PDF Files (*.pdf);;DjVu Files (*.djvu *.djv)")
	);
	if (!fileName.isEmpty())
	{
		openFile(fileName);
	}
}

void MainWindow::openFile(
	const QString & fileName
)
{
	QFileInfo checkFile(fileName);
	if (!checkFile.exists() || !checkFile.isFile())
	{
		qWarning() << "File does not exist:" << fileName;
		QMessageBox::critical(
			this,
			tr("Error"),
			tr("File does not exist: %1").arg(fileName)
		);
		return;
	}

	QSettings settings;
	settings.setValue(
		Settings::LastDir,
		checkFile.absolutePath()
	);

	addRecentFile(checkFile.absoluteFilePath());

	QString suffix = checkFile.suffix().toLower();
	if (suffix == "djvu" || suffix == "djv")
	{
		auto * djvuDocument = new DjvuDocument(this);
		if (djvuDocument->load(fileName))
		{
			qInfo() << "DjVu file loaded:" << fileName;
		}
		else
		{
			qCritical() << "DjVu file load failed:" << fileName;
			QMessageBox::critical(
				this,
				tr("Error"),
				tr("File open error")
			);
			delete djvuDocument;
			return;
		}

		auto * djvuView = new DjvuView(this);
		djvuView->setDocument(djvuDocument);

		connect(
			djvuView,
			&DjvuView::currentPageChanged,
			this,
			&MainWindow::pageNavigator_currentPageChanged
		);

		int tabIndex = centralwidget->addTab(
			djvuView,
			checkFile.fileName()
		);
		centralwidget->setTabToolTip(
			tabIndex,
			checkFile.fileName()
		);
		centralwidget->setCurrentIndex(tabIndex);
	}
	else
	{
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

void MainWindow::updateRecentFilesMenu()
{
	QSettings settings;
	QStringList files = settings.value(
		Settings::RecentFiles
	).toStringList();

	// Remove non-existing files or duplicates if any
	QStringList cleanFiles;
	for (const QString & filePath : files)
	{
		if (QFileInfo::exists(filePath) && !cleanFiles.contains(filePath))
		{
			cleanFiles.append(filePath);
		}
	}
	if (cleanFiles.size() != files.size())
	{
		settings.setValue(
			Settings::RecentFiles,
			cleanFiles
		);
	}

	// Remove previously added recent file actions from menu_File
	for (QAction * action : menu_File->actions())
	{
		if (action->property("recentFile").isValid())
		{
			menu_File->removeAction(action);
			delete action;
		}
	}

	if (!cleanFiles.isEmpty())
	{
		auto * separator = new QAction(menu_File);
		separator->setSeparator(true);
		separator->setProperty("recentFile", true);
		menu_File->addAction(separator);

		for (int i = 0; i < cleanFiles.size(); ++i)
		{
			const QString & filePath = cleanFiles.at(i);
			QString text = QString("&%1 %2").arg(
				i + 1
			).arg(
				QFileInfo(filePath).fileName()
			);
			auto * action = new QAction(
				text,
				this
			);
			action->setData(filePath);
			action->setProperty("recentFile", true);
			action->setToolTip(filePath);
			connect(
				action,
				&QAction::triggered,
				this,
				&MainWindow::openRecentFile
			);
			menu_File->addAction(action);
		}
	}
}

void MainWindow::addRecentFile(
	const QString & fileName
)
{
	QSettings settings;
	QStringList files = settings.value(
		Settings::RecentFiles
	).toStringList();

	files.removeAll(fileName);
	files.prepend(fileName);

	while (files.size() > MAX_RECENT_FILES)
	{
		files.removeLast();
	}

	settings.setValue(
		Settings::RecentFiles,
		files
	);

	updateRecentFilesMenu();
}

void MainWindow::openRecentFile()
{
	auto * action = qobject_cast<QAction *>(sender());
	if (action != nullptr)
	{
		QString fileName = action->data().toString();
		openFile(fileName);
	}
}

void MainWindow::on_centralwidget_currentChanged(int index)
{
	QSignalBlocker blocker0(m_pageSpinBox);
	QSignalBlocker blocker1(m_zoomSpinBox);

	QPdfView * pdfView = currentPdfView();
	QPdfDocument * pdfDocument = currentPdfDocument();
	DjvuView * djvuView = currentDjvuView();
	DjvuDocument * djvuDocument = currentDjvuDocument();

	if (djvuView != nullptr && djvuDocument != nullptr)
	{
		auto * contentsModel = new DjvuContentsModel(this);
		contentsModel->setDocument(djvuDocument);
		contents_treeView->setModel(contentsModel);

		int pageCount = djvuDocument->pageCount();
		m_pageSpinBox->setMaximum(pageCount > 0 ? pageCount - 1 : 0);

		int currentPage = djvuView->currentPage();
		m_pageSpinBox->setValue(currentPage);
		m_zoomSpinBox->setValue(djvuView->zoomFactor() * 100);
		return;
	}

	if (pdfView != nullptr && pdfDocument != nullptr)
	{
		auto * contentsModel = new PdfContentsModel(this);
		contentsModel->setDocument(pdfDocument);
		contents_treeView->setModel(contentsModel);

		int pageCount = pdfDocument->pageCount();
		m_pageSpinBox->setMaximum(pageCount > 0 ? pageCount - 1 : 0);

		int currentPage = pdfView->pageNavigator()->currentPage();
		m_pageSpinBox->setValue(currentPage);
		m_zoomSpinBox->setValue(pdfView->zoomFactor() * 100);
		return;
	}

	contents_treeView->setModel(nullptr);
	m_pageSpinBox->setMaximum(0);
	m_pageSpinBox->setValue(0);
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

	auto * djvuView = dynamic_cast<DjvuView *>(widget);
	if (djvuView != nullptr)
	{
		DjvuDocument * djvuDocument = djvuView->document();
		if (djvuDocument != nullptr)
		{
			delete djvuDocument;
		}
	}

	centralwidget->removeTab(index);
	delete widget;
}

void MainWindow::on_contents_treeView_navigateToPage(int pageNumber) const
{
	QSignalBlocker blocker(m_pageSpinBox);

	if (DjvuView * djvuView = currentDjvuView())
	{
		djvuView->setCurrentPage(pageNumber);
		m_pageSpinBox->setValue(pageNumber);
		return;
	}

	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

	pdfView->pageNavigator()->jump(pageNumber, {});
	m_pageSpinBox->setValue(pageNumber);
}

void MainWindow::on_actionNext_page_triggered(bool checked) const
{
	QSignalBlocker blocker(m_pageSpinBox);

	if (DjvuView * djvuView = currentDjvuView())
	{
		int nextPage = djvuView->currentPage() + 1;
		djvuView->setCurrentPage(nextPage);
		m_pageSpinBox->setValue(djvuView->currentPage());
		return;
	}

	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

	QPdfPageNavigator * navigator = pdfView->pageNavigator();
	int nextPage = navigator->currentPage() + 1;
	navigator->jump(nextPage, {});
	m_pageSpinBox->setValue(nextPage);
}

void MainWindow::on_actionPrev_page_triggered(bool checked) const
{
	QSignalBlocker blocker(m_pageSpinBox);

	if (DjvuView * djvuView = currentDjvuView())
	{
		int prevPage = djvuView->currentPage() - 1;
		if (prevPage >= 0)
		{
			djvuView->setCurrentPage(prevPage);
			m_pageSpinBox->setValue(djvuView->currentPage());
		}
		return;
	}

	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

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
	if (DjvuView * djvuView = currentDjvuView())
	{
		djvuView->setCurrentPage(value);
		return;
	}

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
	// Only update page spinbox if active tab triggered signal
	if (DjvuView * djvuView = currentDjvuView())
	{
		if (sender() == djvuView)
		{
			QSignalBlocker blocker(m_pageSpinBox);
			m_pageSpinBox->setValue(value);
		}
		return;
	}

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
	QSignalBlocker blocker(m_zoomSpinBox);

	if (DjvuView * djvuView = currentDjvuView())
	{
		double zoomFactor = djvuView->zoomFactor() + ZOOM_FACTOR_STEP;
		djvuView->setZoomFactor(zoomFactor);
		m_zoomSpinBox->setValue(zoomFactor * 100);
		return;
	}

	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

	double zoomFactor = pdfView->zoomFactor();
	zoomFactor += ZOOM_FACTOR_STEP;

	pdfView->setZoomFactor(zoomFactor);
	m_zoomSpinBox->setValue(zoomFactor * 100);
}

void MainWindow::on_actionZoom_Out_triggered(bool checked) const
{
	QSignalBlocker blocker(m_zoomSpinBox);

	if (DjvuView * djvuView = currentDjvuView())
	{
		double zoomFactor = djvuView->zoomFactor();
		if (zoomFactor - ZOOM_FACTOR_STEP > 0.0)
		{
			zoomFactor -= ZOOM_FACTOR_STEP;
			djvuView->setZoomFactor(zoomFactor);
			m_zoomSpinBox->setValue(zoomFactor * 100);
		}
		return;
	}

	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}

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
	double zoomFactor = m_zoomSpinBox->value() / 100.0;

	if (DjvuView * djvuView = currentDjvuView())
	{
		djvuView->setZoomFactor(zoomFactor);
		return;
	}

	QPdfView * pdfView = currentPdfView();
	if (pdfView == nullptr)
	{
		return;
	}
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
