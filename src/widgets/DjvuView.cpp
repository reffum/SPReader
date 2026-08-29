//
// DjvuView widget implementation
//

#include <QScrollBar>
#include <QPainter>
#include <QPen>
#include <QFutureWatcher>
#include "DjvuView.h"

// Offset the top page from the top
constexpr int yTopVerticalOffset = 20;
constexpr int yPageInterval = 10;
constexpr int xHorizontalOffset = 10;

constexpr int VERTICAL_SCROLL_STEP = 20;

using namespace std;

DjvuView::DjvuView(QWidget * parent):
	QAbstractScrollArea(parent)
{

}

void DjvuView::recreatePages()
{
	// Store initial information about all pages.
	// Set page coordinate to (0,0) and store width and height
	const int pagesCount = m_document->pageCount();
	pages.clear();
	++m_renderGeneration;

	for (int i = 0; i < pagesCount; ++i)
	{
		QSize pageSize = m_document->pageSize(i) * m_zoomFactor;
		QRect pageRect = QRect(QPoint{0,0}, pageSize);
		Page page = Page{.rect = pageRect, .number = i};

		pages.append(page);
	}
}

void DjvuView::setDocument(DjvuDocument * document)
{
	m_document = document;
	m_currentPage = 0;

	recreatePages();

	rearrangePages();
	setupScrollBars();
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
	assert(pageNumber < m_document->pageCount());

	m_currentPage = pageNumber;

	// Set vertical scroll bar. This force repaint
	// viewport
	Page & page = pages[pageNumber];
	int yPage = page.rect.y();

	verticalScrollBar()->setValue(yPage);
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

	recreatePages();
	rearrangePages();
	setupScrollBars();

	viewport()->update();
}

//
// Return current viewport rect in the origin
//
QRect DjvuView::getViewportRect() const
{
	// The visible area top-left corner is defined by the scroll position
	int x = horizontalScrollBar()->value();
	int y = verticalScrollBar()->value();

	// Visible area size matches the viewport widget size

	return {
		x,
		y,
		viewport()->width(),
		viewport()->height()
		};
}

//
// Return the part of page rect that belongs to the viewport
// Return:
// QRect - part of page visible in viewport in page coord system
// QPoint - coord of visible page's part in viewport coord system
//
optional<pair<QRect, QPoint>> DjvuView::visiblePageRect(
	const QRect & viewportRect,
	const QRect & pageRect)
{
	// Intersect the two rects in document coordinates
	QRect visible = pageRect.intersected(viewportRect);

	if (visible.isNull())
	{
		// The page is not visible at all
		return nullopt;
	}

	// Position of the visible part in the viewport coordinate system
	const QPoint viewportOffset = visible.topLeft() - viewportRect.topLeft();

	// The visible part in the page's own coordinate system
	visible.moveTopLeft(visible.topLeft() - pageRect.topLeft());

	return pair<QRect, QPoint>{visible, viewportOffset};
}

void DjvuView::resizeEvent(QResizeEvent *event)
{
	QAbstractScrollArea::resizeEvent(event);
	setupScrollBars();
}

void DjvuView::paintEvent(QPaintEvent *event)
{
	renderPages();
}

//
// Set pages (x,y) in origin space.
//
void DjvuView::rearrangePages()
{
	// Place pages in 1 vertical column
	int y = qRound(yTopVerticalOffset * m_zoomFactor);
	int x = qRound(xHorizontalOffset * m_zoomFactor);

	int maxPageWidth = 0;

	for (Page& page: pages)
	{
		page.rect.moveTo(x, y);
		int pageH = page.rect.height();

		y += pageH + qRound(yPageInterval * m_zoomFactor);

		maxPageWidth = std::max(page.rect.width(), maxPageWidth);
	}

	totalWidth = maxPageWidth + qRound(2*xHorizontalOffset* m_zoomFactor);
	totalHeight = y;
}

//
// Render visible pages in viewport
//
void DjvuView::renderPage(
	QPainter& painter,
	Page& page,
	QRect pageRect,
	QPoint pageOffset
)
{
	const int pageNumber = page.number;

	// Render the page
	if (page.image.isNull())
	{
		if (!page.isRendering)
		{
			page.isRendering = true;
			const int generation = m_renderGeneration;
			auto * watcher = new QFutureWatcher<QImage>(this);

			connect(
				watcher,
				&QFutureWatcher<QImage>::finished,
				this,
				[this, watcher, pageNumber, generation]()
				{
					if (generation == m_renderGeneration && pageNumber < pages.size())
					{
						pages[pageNumber].image = watcher->result();
						pages[pageNumber].isRendering = false;
						viewport()->update();
					}
					watcher->deleteLater();
				}
			);

			watcher->setFuture(m_document->renderPage(
				pageNumber,
				m_zoomFactor
			));
		}

		painter.save();

		QRect borderRect = QRect(pageOffset, pageRect.size());
		painter.fillRect(borderRect, Qt::white);
		painter.setPen(QPen(Qt::black, 1));
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(borderRect);

		painter.restore();
		return;
	}

	const QImage & image = page.image;

	painter.save();

	painter.drawImage(pageOffset, image, pageRect);

	// Render borderline around this page
	QRect borderRect = QRect(pageOffset, pageRect.size());
	painter.setPen(QPen(Qt::black, 1));
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(borderRect);

	painter.restore();
}

//
// Redraw all pages
//
void DjvuView::renderPages()
{
	QPainter painter(viewport());

	// Fill background
	painter.fillRect(rect(), Qt::lightGray);

	QRect viewportRect = getViewportRect();

	QList<Page> visiblePages;

	for (Page& page: pages)
	{
		QRect pageRect = page.rect;
		auto pageParameters = visiblePageRect(
			viewportRect,
			pageRect
			);
		if (pageParameters.has_value())
		{
			auto [pagePartRect, pageOffset] = pageParameters.value();
			renderPage(painter, page, pagePartRect, pageOffset);

			visiblePages.append(page);
		}
	}

	// Set number of the top visible page as a new current page
	// and emit currentPageChanged if it has changed
	if (!visiblePages.isEmpty())
	{
		const int newCurrentPage = visiblePages.front().number;
		if (m_currentPage != newCurrentPage)
		{
			m_currentPage = newCurrentPage;
			emit currentPageChanged(m_currentPage);
		}
	}
}

void DjvuView::setupScrollBars() const
{
	int viewportHeight = viewport()->height();
	int viewportWidth = viewport()->width();

	int rangeHeight = std::max(0, totalHeight - viewportHeight);
	int rangeWidth = std::max(0, totalWidth - viewportWidth);

	verticalScrollBar()->setRange(0, rangeHeight);
	verticalScrollBar()->setPageStep(viewportHeight);
	verticalScrollBar()->setSingleStep(VERTICAL_SCROLL_STEP);

	horizontalScrollBar()->setRange(0, rangeWidth);
	horizontalScrollBar()->setPageStep(viewportWidth);
}
