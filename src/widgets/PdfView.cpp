//
// Created by oleg on 9/12/26.
//

#include "PdfView.h"

#include <QAction>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPdfDocument>
#include <QPdfDocumentRenderOptions>
#include <QPdfPageNavigator>
#include <QPdfSelection>
#include <QScreen>
#include <QScrollBar>
#include <algorithm>

PdfView::PdfView(QWidget *parent):
	QPdfView(parent)
{
	setMouseTracking(true);
	setMode(Mode::Hand);
}

void PdfView::setMode(Mode mode)
{
	m_mode = mode;
	m_dragging = false;
	clearSelection();

	switch (m_mode)
	{
		case Mode::Hand:
			viewport()->setCursor(Qt::OpenHandCursor);
			break;
		case Mode::SelectText:
			viewport()->setCursor(Qt::IBeamCursor);
			break;
		case Mode::SelectImage:
			viewport()->setCursor(Qt::CrossCursor);
			break;
	}
}

PdfView::Mode PdfView::mode() const
{
	return m_mode;
}

void PdfView::clearSelection()
{
	m_selectedPage = -1;
	m_startPagePt = {};
	m_endPagePt = {};
	m_imageRectPts = {};
	viewport()->update();
}

void PdfView::copySelectionToClipboard() const
{
	if (!document() || m_selectedPage < 0)
	{
		return;
	}

	if (m_mode == Mode::SelectText)
	{
		QPdfSelection sel = document()->getSelection(
			m_selectedPage,
			m_startPagePt,
			m_endPagePt
		);
		if (sel.isValid() && !sel.text().isEmpty())
		{
			sel.copyToClipboard();
		}
	}
	else if (m_mode == Mode::SelectImage && !m_imageRectPts.isEmpty())
	{
		QImage img = renderSelectedImage();
		if (!img.isNull())
		{
			QGuiApplication::clipboard()->setImage(img);
		}
	}
}

void PdfView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton || !document())
	{
		QPdfView::mousePressEvent(event);
		return;
	}

	m_dragging = true;
	m_dragStartPos = event->pos();

	if (m_mode == Mode::Hand)
	{
		m_hScrollStart = horizontalScrollBar()->value();
		m_vScrollStart = verticalScrollBar()->value();
		viewport()->setCursor(Qt::ClosedHandCursor);
	}
	else
	{
		int page = -1;
		QPointF pagePt;
		if (mapViewportToPage(event->pos(), page, pagePt))
		{
			m_selectedPage = page;
			m_startPagePt = pagePt;
			m_endPagePt = pagePt;
			m_imageRectPts = QRectF(pagePt, pagePt);
		}
		else
		{
			clearSelection();
			m_dragging = false;
		}
		viewport()->update();
	}

	event->accept();
}

void PdfView::mouseMoveEvent(QMouseEvent *event)
{
	if (!m_dragging || !document())
	{
		QPdfView::mouseMoveEvent(event);
		return;
	}

	if (m_mode == Mode::Hand)
	{
		const QPoint delta = event->pos() - m_dragStartPos;
		horizontalScrollBar()->setValue(m_hScrollStart - delta.x());
		verticalScrollBar()->setValue(m_vScrollStart - delta.y());
	}
	else if (m_selectedPage >= 0)
	{
		QPointF pagePt = mapViewportToSpecificPage(
			event->pos(),
			m_selectedPage
		);
		m_endPagePt = pagePt;
		m_imageRectPts = QRectF(m_startPagePt, m_endPagePt).normalized();
		viewport()->update();
	}

	event->accept();
}

void PdfView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_dragging && event->button() == Qt::LeftButton)
	{
		m_dragging = false;
		if (m_mode == Mode::Hand)
		{
			viewport()->setCursor(Qt::OpenHandCursor);
		}
		event->accept();
		return;
	}

	QPdfView::mouseReleaseEvent(event);
}

void PdfView::keyPressEvent(QKeyEvent *event)
{
	if (event->matches(QKeySequence::Copy))
	{
		copySelectionToClipboard();
		event->accept();
		return;
	}
	if (event->key() == Qt::Key_Escape)
	{
		clearSelection();
		event->accept();
		return;
	}

	QPdfView::keyPressEvent(event);
}

void PdfView::contextMenuEvent(QContextMenuEvent *event)
{
	if (!document() || m_selectedPage < 0)
	{
		return;
	}

	QMenu menu(this);
	if (m_mode == Mode::SelectText)
	{
		QPdfSelection sel = document()->getSelection(
			m_selectedPage,
			m_startPagePt,
			m_endPagePt
		);
		if (sel.isValid() && !sel.text().isEmpty())
		{
			QAction *copyAct = menu.addAction(tr("Copy Text"));
			connect(
				copyAct,
				&QAction::triggered,
				this,
				&PdfView::copySelectionToClipboard
			);
		}
	}
	else if (m_mode == Mode::SelectImage && !m_imageRectPts.isEmpty())
	{
		QAction *copyImgAct = menu.addAction(tr("Copy Image"));
		connect(
			copyImgAct,
			&QAction::triggered,
			this,
			&PdfView::copySelectionToClipboard
		);
	}

	if (!menu.isEmpty())
	{
		menu.exec(event->globalPos());
	}
}

void PdfView::paintEvent(QPaintEvent *event)
{
	QPdfView::paintEvent(event);

	if (!document() || m_selectedPage < 0)
	{
		return;
	}

	PageGeometry geom = pageGeometry(m_selectedPage);
	if (geom.scale <= 0.0)
	{
		return;
	}

	const QPoint scrollOffset(
		horizontalScrollBar()->value(),
		verticalScrollBar()->value()
	);
	const QPointF pageOriginInViewport =
		geom.rectInDoc.topLeft() - scrollOffset;

	QPainter painter(viewport());
	painter.setRenderHint(QPainter::Antialiasing, true);

	if (m_mode == Mode::SelectText)
	{
		QPdfSelection sel = document()->getSelection(
			m_selectedPage,
			m_startPagePt,
			m_endPagePt
		);
		if (!sel.isValid())
		{
			return;
		}

		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(0, 120, 215, 80));

		for (const QPolygonF &poly : sel.bounds())
		{
			QPolygonF vpPoly;
			vpPoly.reserve(poly.size());
			for (const QPointF &pt : poly)
			{
				vpPoly.append(pt * geom.scale + pageOriginInViewport);
			}
			painter.drawPolygon(vpPoly);
		}
	}
	else if (m_mode == Mode::SelectImage && !m_imageRectPts.isEmpty())
	{
		QRectF vpRect(
			m_imageRectPts.topLeft() * geom.scale + pageOriginInViewport,
			m_imageRectPts.size() * geom.scale
		);
		painter.setPen(QPen(QColor(0, 120, 215), 1, Qt::DashLine));
		painter.setBrush(QColor(0, 120, 215, 40));
		painter.drawRect(vpRect);
	}
}

qreal PdfView::screenResolution() const
{
	QScreen *scr = QGuiApplication::primaryScreen();
	const qreal dpi = scr ? scr->logicalDotsPerInch() : 96.0;
	return dpi / 72.0;
}

qreal PdfView::scaleForPage(int page) const
{
	const QSizeF ptSize = document()->pagePointSize(page);
	if (ptSize.isEmpty())
	{
		return 0.0;
	}

	const QMargins margins = documentMargins();
	const int availW = viewport()->width() - margins.left() - margins.right();
	const int availH = viewport()->height() - margins.top() - margins.bottom();

	switch (zoomMode())
	{
		case ZoomMode::Custom:
			return screenResolution() * zoomFactor();
		case ZoomMode::FitToWidth:
			return availW > 0 ? (qreal(availW) / ptSize.width()) : 0.0;
		case ZoomMode::FitInView:
		{
			if (availW <= 0 || availH <= 0)
			{
				return 0.0;
			}
			const qreal sx = qreal(availW) / ptSize.width();
			const qreal sy = qreal(availH) / (ptSize.height() + pageSpacing());
			return std::min(sx, sy);
		}
	}
	return screenResolution() * zoomFactor();
}

PdfView::PageGeometry PdfView::pageGeometry(int targetPage) const
{
	if (!document() || targetPage < 0 || targetPage >= document()->pageCount())
	{
		return {};
	}

	const QMargins margins = documentMargins();
	const int startPage = (pageMode() == PageMode::SinglePage)
		? pageNavigator()->currentPage()
		: 0;
	const int endPage = (pageMode() == PageMode::SinglePage)
		? startPage + 1
		: document()->pageCount();

	int totalWidth = viewport()->width();
	for (int i = startPage; i < endPage; ++i)
	{
		const qreal s = scaleForPage(i);
		const QSize pxSize = (document()->pagePointSize(i) * s).toSize();
		totalWidth = std::max(
			totalWidth,
			pxSize.width() + margins.left() + margins.right()
		);
	}

	int y = margins.top();
	for (int i = startPage; i < endPage; ++i)
	{
		const qreal s = scaleForPage(i);
		const QSize pxSize = (document()->pagePointSize(i) * s).toSize();
		const int x = (
			totalWidth - pxSize.width() + margins.left() - margins.right()
		) / 2;

		const QRect pageRect(QPoint(x, y), pxSize);
		if (i == targetPage)
		{
			return PageGeometry{pageRect, s};
		}

		y += pxSize.height() + pageSpacing();
	}

	return {};
}

bool PdfView::mapViewportToPage(
	const QPoint &viewportPos,
	int &outPage,
	QPointF &outPagePt
) const
{
	if (!document())
	{
		return false;
	}

	const QPoint docPos(
		viewportPos.x() + horizontalScrollBar()->value(),
		viewportPos.y() + verticalScrollBar()->value()
	);

	const int startPage = (pageMode() == PageMode::SinglePage)
		? pageNavigator()->currentPage()
		: 0;
	const int endPage = (pageMode() == PageMode::SinglePage)
		? startPage + 1
		: document()->pageCount();

	for (int i = startPage; i < endPage; ++i)
	{
		PageGeometry geom = pageGeometry(i);
		if (geom.scale > 0.0 && geom.rectInDoc.contains(docPos))
		{
			outPage = i;
			outPagePt = QPointF(docPos - geom.rectInDoc.topLeft()) / geom.scale;
			return true;
		}
	}

	return false;
}

QPointF PdfView::mapViewportToSpecificPage(
	const QPoint &viewportPos,
	int page
) const
{
	PageGeometry geom = pageGeometry(page);
	if (geom.scale <= 0.0)
	{
		return {};
	}

	const QPoint docPos(
		viewportPos.x() + horizontalScrollBar()->value(),
		viewportPos.y() + verticalScrollBar()->value()
	);
	const QSizeF ptSize = document()->pagePointSize(page);
	QPointF rawPt = QPointF(docPos - geom.rectInDoc.topLeft()) / geom.scale;

	return QPointF(
		std::clamp(rawPt.x(), 0.0, ptSize.width()),
		std::clamp(rawPt.y(), 0.0, ptSize.height())
	);
}

QImage PdfView::renderSelectedImage() const
{
	if (!document() || m_selectedPage < 0 || m_imageRectPts.isEmpty())
	{
		return {};
	}

	constexpr qreal renderScale = 300.0 / 72.0;
	const QSizeF pagePtSize = document()->pagePointSize(m_selectedPage);
	const QSize fullScaledSize = (pagePtSize * renderScale).toSize();
	const QRect clipRect = QRectF(
		m_imageRectPts.topLeft() * renderScale,
		m_imageRectPts.size() * renderScale
	).toRect().intersected(QRect(QPoint(0, 0), fullScaledSize));

	if (clipRect.isEmpty())
	{
		return {};
	}

	QPdfDocumentRenderOptions options;
	options.setScaledSize(fullScaledSize);
	options.setScaledClipRect(clipRect);

	return document()->render(m_selectedPage, clipRect.size(), options);
}
