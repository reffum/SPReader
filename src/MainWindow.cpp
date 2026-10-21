//
// The application main window
//
#include "MainWindow.h"
#include "SettingsManager.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QPdfPageNavigator>
#include <QMessageBox>
#include <QSpinBox>

#include "data/DjvuDocument.h"
#include "models/DjvuContentsModel.h"
#include "widgets/DocumentWidget.h"

using namespace std;

constexpr double CONTENTS_DOCKWIDGET_PART = 0.3;
constexpr double DEFAULT_ZOOM_FACTOR = 1.0;
constexpr double MAXIMUM_ZOOM_FACTOR = 5.0;
constexpr double ZOOM_FACTOR_STEP = 0.25;
constexpr int MAX_RECENT_FILES = 10;

MainWindow::MainWindow(QWidget * parent):
	QMainWindow(parent)
{
	setupUi(this);

	updateRecentFilesMenu();

	// Setup contents QDockWidget parameters
	int contents_dockWidgetSize = static_cast<int>(
		width() * CONTENTS_DOCKWIDGET_PART
	);
	resizeDocks(
		{ contents_dockWidget } ,
		{ contents_dockWidgetSize } ,
		Qt::Horizontal
	);

	// Add pages spin box
	m_pageSpinBox = new QSpinBox(this);
	toolBar->insertWidget(
		actionNext_page,
		m_pageSpinBox
	);

	// Add zoom spin box
	m_zoomSpinBox = new QDoubleSpinBox(this);
	toolBar->insertWidget(
		actionZoom_Out,
		m_zoomSpinBox
	);
	m_zoomSpinBox->setMaximum(
		MAXIMUM_ZOOM_FACTOR * 100
	);
	m_zoomSpinBox->setMinimum(
		ZOOM_FACTOR_STEP * 100.0
	);
	m_zoomSpinBox->setValue(
		DEFAULT_ZOOM_FACTOR * 100
	);
	m_zoomSpinBox->setSingleStep(
		ZOOM_FACTOR_STEP * 100
	);

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

	// Setup maximum width of tab title
	centralTab->setElideMode(Qt::ElideRight);

	centralTab->setStyleSheet(
		"QTabBar::tab { max-width: " +
		QString::number(centralTab->width() / 3) +
		"px; }"
	);
}


void MainWindow::openFile(const QString & fileName)
{
	try
	{
		DocumentWidget* widget = DocumentWidget::openDocument(fileName);

		// Add new document
		const int tabIndex = centralTab->addTab(
			widget,
			widget->getFileName()
		);

		centralTab->setTabToolTip(tabIndex, widget->getFullFileName());
		centralTab->setCurrentIndex(tabIndex);

		connect(
			widget,
			SIGNAL(currentPageChanged(int)),
			this,
			SLOT(currentDocument_currentPageChanged(int))
		);

		connect(
			widget,
			SIGNAL(zoomChanged(qreal)),
			this,
			SLOT(currentDocument_zoomChanged(qreal))
		);
	}
	catch (OpenDocumentError & e)
	{
		QMessageBox::critical(this, "Open document error", e.what());
	}
}

void MainWindow::updateRecentFilesMenu()
{
	QStringList cleanFiles =
		SettingsManager::getRecentFiles();

	// Remove previously added recent file
	// actions from menu_File
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
				&MainWindow::openRecentFile_action_triggered
			);
			menu_File->addAction(action);
		}
	}
}

void MainWindow::addRecentFile(
	const QString & fileName
)
{
	SettingsManager::addRecentFile(fileName);
	updateRecentFilesMenu();
}

//
// Auto-connected slots
//

void MainWindow::on_action_Open_triggered(bool)
{
	QString lastDir = SettingsManager::getLastDir();
	if (lastDir.isEmpty())
	{
		lastDir = QDir::homePath();
	}

	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Open Document"),
		lastDir,
		tr(
			"Supported Files (*.pdf *.djvu *.djv);;"
			"PDF Files (*.pdf);;"
			"DjVu Files (*.djvu *.djv)"
		)
	);
	if (!fileName.isEmpty())
	{
		openFile(fileName);
	}
}

void MainWindow::setCurrentDocumentPage(int pageNumber) const
{
	QSignalBlocker blocker(m_pageSpinBox);

	auto * documentWidget = qobject_cast<DocumentWidget *>(centralTab->currentWidget());

	if (pageNumber >= documentWidget->getPageCount())
	{
		qWarning() << "Contents page number " << pageNumber << " is out of range("
		<< documentWidget->getPageCount() << ")";

		return;
	}

	documentWidget->setCurrentPage(pageNumber);
	m_pageSpinBox->setValue(pageNumber);
}

void MainWindow::on_actionAbout_triggered(bool checked)
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

void MainWindow::on_centralTab_currentChanged(int) const
{
	QSignalBlocker blocker0(m_pageSpinBox);
	QSignalBlocker blocker1(m_zoomSpinBox);

	auto * documentWidget = qobject_cast<DocumentWidget *>(centralTab->currentWidget());

	int currentPage = documentWidget->getCurrentPage();
	int pageCount = documentWidget->getPageCount();
	qreal zoomFactor = documentWidget->getZoomFactor();

	QAbstractItemModel * contentsModel = documentWidget->contentsModel();

	m_pageSpinBox->setMaximum(pageCount);
	m_pageSpinBox->setValue(currentPage);
	m_zoomSpinBox->setValue(zoomFactor);

	contents_treeView->setModel(contentsModel);
}

void MainWindow::on_contents_treeView_navigateToPage(int pageNumber) const
{
	setCurrentDocumentPage(pageNumber);
}

void MainWindow::on_actionNext_page_triggered(bool) const
{
	int currentPage = m_pageSpinBox->value();

	if (currentPage == m_pageSpinBox->maximum())
	{
		return;
	}

	setCurrentDocumentPage(currentPage + 1);
}

void MainWindow::on_actionPrev_page_triggered(bool) const
{
	int currentPage = m_pageSpinBox->value();
	if (currentPage == m_pageSpinBox->minimum())
	{
		return;
	}

	setCurrentDocumentPage(currentPage - 1);
}

void MainWindow::on_actionZoom_In_triggered(bool) const
{
	QSignalBlocker blocker(m_zoomSpinBox);

	auto * documentWidget = qobject_cast<DocumentWidget *>(centralTab->currentWidget());
	qreal zoomFactor = documentWidget->getZoomFactor() + ZOOM_FACTOR_STEP;
	documentWidget->setZoomFactor(zoomFactor);

	m_zoomSpinBox->setValue(zoomFactor * 100);
}

void MainWindow::on_actionZoom_Out_triggered(bool) const
{
	QSignalBlocker blocker(m_zoomSpinBox);

	auto * documentWidget = qobject_cast<DocumentWidget *>(centralTab->currentWidget());
	qreal zoomFactor = documentWidget->getZoomFactor() - ZOOM_FACTOR_STEP;

	if (zoomFactor - ZOOM_FACTOR_STEP > 0.0)
	{
		documentWidget->setZoomFactor(zoomFactor);
		m_zoomSpinBox->setValue(zoomFactor * 100);
	}
}

void MainWindow::on_centralTab_tabCloseRequested(int index) const
{
	auto * documentWidget = qobject_cast<DocumentWidget *>(centralTab->currentWidget());
	centralTab->removeTab(index);

	delete documentWidget;
}

void MainWindow::on_actionContents_triggered(bool checked) const
{
	this->contents_dockWidget->setVisible(checked);
}

//
// Manually connected slots
//
void MainWindow::currentDocument_currentPageChanged(int value) const
{
	QSignalBlocker blocker(m_pageSpinBox);
	m_pageSpinBox->setValue(value);
}

void MainWindow::currentDocument_zoomChanged(qreal value) const
{
	QSignalBlocker blocker(m_zoomSpinBox);
	m_zoomSpinBox->setValue(value);
}

void MainWindow::m_pageSpinBox_valueChanged(int value) const
{
	int pageNumber = value;
	auto * documentWidget = qobject_cast<DocumentWidget *>(centralTab->currentWidget());

	assert(value <= documentWidget->getPageCount());

	documentWidget->setCurrentPage(pageNumber);
	m_pageSpinBox->setValue(pageNumber);
}

void MainWindow::m_zoomSpinBox_valueChanged(double) const
{
	double zoomFactor = m_zoomSpinBox->value() / 100.0;

	auto * documentWidget = qobject_cast<DocumentWidget *>(centralTab->currentWidget());
	documentWidget->setZoomFactor(zoomFactor);
}

void MainWindow::openRecentFile_action_triggered(bool)
{
	auto * action = qobject_cast<QAction *>(sender());
	if (action != nullptr)
	{
		QString fileName = action->data().toString();
		openFile(fileName);
	}
}

//
// Events
//

void MainWindow::resizeEvent(QResizeEvent * event)
{
	QMainWindow::resizeEvent(event);
	centralTab->setStyleSheet(
		"QTabBar::tab { max-width: " +
		QString::number(centralTab->width() / 4) +
		"px; }"
	);
}
