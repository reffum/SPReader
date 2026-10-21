//
// Created by oleg on 10/18/26.
//
#include <QPdfDocument>
#include <QVBoxLayout>

#include "DocumentWidget.h"
#include "PdfWidget.h"

#include <QPdfPageNavigator>

#include "PdfView.h"
#include "SettingsManager.h"
#include "models/PdfContentsModel.h"

using namespace std;

PdfWidget::PdfWidget(const QString& fileName, QWidget* parent)
	: DocumentWidget(fileName, parent)
{
	using enum QPdfDocument::Error;

	QPdfDocument * pdfDocument = new QPdfDocument(this);
	QPdfDocument::Error r = pdfDocument->load(fileName);

	if (r != None)
	{
		QString errorMessage;
		switch (r)
		{
			case DataNotYetAvailable: errorMessage = "Data not yet available"; break;
			case FileNotFound: errorMessage = "File not found"; break;
			case InvalidFileFormat: errorMessage = "Invalid file format"; break;
			default: errorMessage = "Unknown error"; break;
		}

		qCritical() << "Open file " << fileName << " failed: " << errorMessage << "Error code: " << r;
		delete pdfDocument;
		throw OpenDocumentError(errorMessage);
	}

	m_pdfView = new PdfView(this);

	m_pdfView->setDocument(pdfDocument);
	m_pdfView->setPageMode(QPdfView::PageMode::MultiPage);

	QVBoxLayout* rootLayout = new QVBoxLayout(this);
	rootLayout->setSpacing(0);
	rootLayout->setContentsMargins(0,0,0,0);
	rootLayout->addWidget(m_pdfView);

	m_pdfContentsModel = new PdfContentsModel(m_pdfView);
	m_pdfContentsModel->setDocument(pdfDocument);

	// Restore settings
	optional<int> lastPage = SettingsManager::getLastPage(m_fullFileName);
	optional<double> zoom = SettingsManager::getZoom(m_fullFileName);

	if (lastPage.has_value())
	{
		m_pdfView->pageNavigator()->jump(lastPage.value(), {});
	}

	if (zoom.has_value())
	{
		m_pdfView->setZoomFactor(zoom.value());
	}

	connect(
		m_pdfView->pageNavigator(),
		&QPdfPageNavigator::currentPageChanged,
		this,
		[this](int value){ emit currentPageChanged(value); }
	);
}

PdfWidget::~PdfWidget()
{
	int page = m_pdfView->pageNavigator()->currentPage();
	qreal zoom = m_pdfView->zoomFactor();

	SettingsManager::setLastPage(m_fullFileName, page);
	SettingsManager::setZoom(m_fullFileName, zoom);
}

int PdfWidget::getCurrentPage() const
{
	return m_pdfView->pageNavigator()->currentPage();
}

int PdfWidget::getPageCount() const
{
	return m_pdfView->document()->pageCount();
}

qreal PdfWidget::getZoomFactor() const
{
	return m_pdfView->zoomFactor();
}

void PdfWidget::setZoomFactor(qreal zoom)
{
	m_pdfView->setZoomFactor(zoom);
}

void PdfWidget::setCurrentPage(int page)
{
	m_pdfView->pageNavigator()->jump(page, {});
}

QAbstractItemModel * PdfWidget::contentsModel() const
{
	return m_pdfContentsModel;
}

QAbstractItemModel * PdfWidget::bookmarksModel() const
{
	//TODO: not implemented yet
	return nullptr;
}
