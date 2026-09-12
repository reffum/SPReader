//
// Created by oleg on 9/12/26.
//

#include <QScrollBar>
#include <QMouseEvent>
#include "PdfView.h"

PdfView::PdfView(QWidget *parent):
	QPdfView(parent)
{
	setMouseTracking(true);
	viewport()->setCursor(Qt::OpenHandCursor);
}

void PdfView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_dragging = true;
		m_dragStartPos = event->pos();
		m_hScrollStart = horizontalScrollBar()->value();
		m_vScrollStart = verticalScrollBar()->value();
		viewport()->setCursor(Qt::ClosedHandCursor);
		event->accept();
	} else {
		QPdfView::mousePressEvent(event);
	}
}

void PdfView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_dragging) {
		const QPoint delta = event->pos() - m_dragStartPos;
		horizontalScrollBar()->setValue(m_hScrollStart - delta.x());
		verticalScrollBar()->setValue(m_vScrollStart - delta.y());
		event->accept();
	} else {
		QPdfView::mouseMoveEvent(event);
	}
}

void PdfView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_dragging && event->button() == Qt::LeftButton) {
		m_dragging = false;
		viewport()->setCursor(Qt::OpenHandCursor);
		event->accept();
	} else {
		QPdfView::mouseReleaseEvent(event);
	}
}
