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

void PdfWidget::setMode(Mode mode)
{
	switch (mode)
	{
		case Mode::Hand:
			m_pdfView->setMode(PdfView::Mode::Hand);
			break;
		case Mode::SelectText:
			m_pdfView->setMode(PdfView::Mode::SelectText);
			break;
		case Mode::SelectImage:
			m_pdfView->setMode(PdfView::Mode::SelectImage);
			break;
	}
}

DocumentWidget::Mode PdfWidget::getMode()
{
	switch (m_pdfView->mode())
	{
		case PdfView::Mode::Hand:
			return Mode::Hand;
		case PdfView::Mode::SelectText:
			return Mode::SelectText;
		case PdfView::Mode::SelectImage:
			return Mode::SelectImage;
		default:
			assert(false);
	}
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
	DocumentWidget::setCurrentPage(page);
	m_pdfView->pageNavigator()->jump(page, {});
}

QAbstractItemModel * PdfWidget::contentsModel() const
{
	return m_pdfContentsModel;
}

void PdfWidget::setMode(PdfView::Mode mode)
{
	m_pdfView->setMode(mode);
}

PdfView::Mode PdfWidget::mode() const
{
	return m_pdfView->mode();
}

void PdfWidget::copySelectionToClipboard() const
{
	m_pdfView->copySelectionToClipboard();
}

PdfView * PdfWidget::pdfView() const
{
	return m_pdfView;
}
