//
// Created by oleg on 10/21/26.
//

#include "DjvuWidget.h"

#include <QVBoxLayout>

#include "DjvuView.h"
#include "SettingsManager.h"
#include "data/DjvuDocument.h"
#include "models/DjvuContentsModel.h"

class QVBoxLayout;
using namespace std;

DjvuWidget::DjvuWidget(const QString& fileName, QWidget* parent)
	: DocumentWidget(fileName, parent)
{
	auto * djvuDocument = new DjvuDocument(this);
	bool r = djvuDocument->load(fileName);

	if (!r)
	{
		throw OpenDocumentError("");
	}

	m_djvuView = new DjvuView(this);
	m_djvuView->setDocument(djvuDocument);

	connect(
		m_djvuView,
		&DjvuView::currentPageChanged,
		this,
		[this](int value){ emit currentPageChanged(value); }
	);

	QVBoxLayout* rootLayout = new QVBoxLayout(this);
	rootLayout->setSpacing(0);
	rootLayout->setContentsMargins(0,0,0,0);
	rootLayout->addWidget(m_djvuView);

	m_djvuContentsModel = new DjvuContentsModel(m_djvuView);

	// Restore settings
	optional<int> lastPage = SettingsManager::getLastPage(m_fullFileName);
	optional<double> zoom = SettingsManager::getZoom(m_fullFileName);

	if (lastPage.has_value())
	{
		m_djvuView->setCurrentPage(lastPage.value());
	}

	if (zoom.has_value())
	{
		m_djvuView->setZoomFactor(zoom.value());
	}
}

DjvuWidget::~DjvuWidget()
{
	int page = m_djvuView->currentPage();
	qreal zoom = m_djvuView->zoomFactor();

	SettingsManager::setLastPage(m_fullFileName, page);
	SettingsManager::setZoom(m_fullFileName, zoom);
}

int DjvuWidget::getCurrentPage() const
{
	return m_djvuView->currentPage();
}

int DjvuWidget::getPageCount() const
{
	return m_djvuView->document()->pageCount();
}

qreal DjvuWidget::getZoomFactor() const
{
	return m_djvuView->zoomFactor();
}

void DjvuWidget::setZoomFactor(qreal zoom)
{
	m_djvuView->setZoomFactor(zoom);
}

void DjvuWidget::setCurrentPage(int page)
{
	m_djvuView->setCurrentPage(page);
}

QAbstractItemModel * DjvuWidget::contentsModel() const
{
	return m_djvuContentsModel;
}

QAbstractItemModel * DjvuWidget::bookmarksModel() const
{
	//TODO: not implemented yet
	return nullptr;
}
