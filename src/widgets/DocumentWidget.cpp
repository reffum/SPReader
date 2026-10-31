//
// Created by oleg on 10/18/26.
//
#include "DocumentWidget.h"

#include <QFileInfo>

#include "PdfWidget.h"
#include "DjvuWidget.h"
#include "SettingsManager.h"
#include "models/BookmarksModel.h"

DocumentWidget* DocumentWidget::openDocument(const QString &fileName)
{
	if (fileName.endsWith(".pdf", Qt::CaseInsensitive))
	{
		return new PdfWidget(fileName);
	}

	if (fileName.endsWith(".djvu", Qt::CaseInsensitive))
	{
		return new DjvuWidget(fileName);
	}

	throw OpenDocumentError("Unsupported document type");
}

DocumentWidget::DocumentWidget(const QString& fileName, QWidget* parent):
	QWidget(parent),
	m_fullFileName(fileName)
{
	QString bookmarksFile = SettingsManager::getBookmarksFile(fileName);
	m_bookmarksModel = new BookmarksModel(bookmarksFile);

	connect(this, SIGNAL(currentPageChanged(int)), this, SLOT(on_currentPageChanged(int)));
}

void DocumentWidget::setCurrentPage(int page)
{
	m_bookmarksModel->setCurrentPage(page);
}

QAbstractItemModel * DocumentWidget::bookmarksModel() const
{
	return m_bookmarksModel;
}

QString DocumentWidget::getFileName() const
{
	QFileInfo fileInfo(m_fullFileName);
	return fileInfo.fileName();
}

QString DocumentWidget::getFullFileName() const
{
	return m_fullFileName;
}

void DocumentWidget::on_currentPageChanged(int page) const
{
	m_bookmarksModel->setCurrentPage(page);
}
