//
// DjvuView widget implementation
//
#include "DjvuView.h"
#include <QVBoxLayout>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QFutureWatcher>

DjvuView::DjvuView(QWidget * parent):
	QScrollArea(parent)
{
	m_containerWidget = new QWidget(this);
	m_layout = new QVBoxLayout(m_containerWidget);
	m_layout->setAlignment(Qt::AlignHCenter);
	m_layout->setContentsMargins(10, 10, 10, 10);
	m_layout->setSpacing(10);
	m_containerWidget->setLayout(m_layout);

	setWidget(m_containerWidget);
	setWidgetResizable(true);
	setAlignment(Qt::AlignCenter);
}

void DjvuView::clearPages()
{
	m_pageLabels.clear();
	QLayoutItem * item;
	while ((item = m_layout->takeAt(0)) != nullptr)
	{
		delete item->widget();
		delete item;
	}
}

void DjvuView::setDocument(DjvuDocument * document)
{
	m_document = document;
	m_currentPage = 0;
	renderPages();
}

DjvuDocument * DjvuView::document() const
{
	return m_document;
}

int DjvuView::currentPage() const
{
	return m_currentPage;
}

void DjvuView::setCurrentPage(int pageNumber)
{
	if (m_document == nullptr || pageNumber < 0 || pageNumber >= m_document->pageCount())
	{
		return;
	}

	m_currentPage = pageNumber;
	if (pageNumber < m_pageLabels.size() && m_pageLabels[pageNumber] != nullptr)
	{
		QLabel * target = m_pageLabels[pageNumber];
		ensureWidgetVisible(target, 0, 10);
	}
	emit currentPageChanged(m_currentPage);
}

double DjvuView::zoomFactor() const
{
	return m_zoomFactor;
}

void DjvuView::setZoomFactor(double factor)
{
	if (factor <= 0.0)
	{
		return;
	}
	m_zoomFactor = factor;
	renderPages();
}

void DjvuView::renderPages()
{
	clearPages();

	if (m_document == nullptr)
	{
		return;
	}

	int count = m_document->pageCount();
	for (int i = 0; i < count; ++i)
	{
		// ReSharper disable once CppDFAMemoryLeak
		auto * label = new QLabel(m_containerWidget);
		label->setAlignment(Qt::AlignCenter);
		m_layout->addWidget(label);
		m_pageLabels.append(label);

		auto * watcher = new QFutureWatcher<QImage>(label);
		connect(
			watcher,
			&QFutureWatcher<QImage>::finished,
			this,
			[label, watcher]()
			{
				QImage img = watcher->result();
				if (!img.isNull())
				{
					label->setPixmap(QPixmap::fromImage(img));
				}
				watcher->deleteLater();
			}
		);
		watcher->setFuture(m_document->renderPage(i, m_zoomFactor));
	}

	if (m_currentPage >= count)
	{
		m_currentPage = 0;
	}
}

void DjvuView::scrollContentsBy(
	int dx,
	int dy
)
{
	QScrollArea::scrollContentsBy(dx, dy);
	updateCurrentPageFromScroll();
}

void DjvuView::updateCurrentPageFromScroll()
{
	if (m_pageLabels.isEmpty())
	{
		return;
	}

	int viewportCenterY = viewport()->rect().center().y();

	int bestPage = m_currentPage;
	int minDistance = std::numeric_limits<int>::max();

	for (int i = 0; i < m_pageLabels.size(); ++i)
	{
		QLabel * label = m_pageLabels[i];
		QRect rect = label->geometry();
		QRect mappedRect = QRect(
			label->mapTo(viewport(), QPoint(0, 0)),
			rect.size()
		);

		int labelCenterY = mappedRect.center().y();
		int distance = std::abs(labelCenterY - viewportCenterY);
		if (distance < minDistance)
		{
			minDistance = distance;
			bestPage = i;
		}
	}

	if (bestPage != m_currentPage)
	{
		m_currentPage = bestPage;
		emit currentPageChanged(m_currentPage);
	}
}
