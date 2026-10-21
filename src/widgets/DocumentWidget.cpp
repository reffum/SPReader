//
// Created by oleg on 10/18/26.
//
#include "DocumentWidget.h"

#include <QFileInfo>

#include "PdfWidget.h"
#include "DjvuWidget.h"

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
