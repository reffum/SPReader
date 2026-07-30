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
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent),
	m_pdfDocument(new QPdfDocument(this))
{
	setupUi(this);
	auto pdfView = static_cast<QPdfView *>(centralWidget());
	pdfView->setDocument(m_pdfDocument);
	pdfView->setPageMode(QPdfView::PageMode::MultiPage);
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
	}
}
