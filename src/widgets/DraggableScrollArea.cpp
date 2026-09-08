//
// DraggableScrollArea widget implementation
//

#include <QScrollBar>
#include <QMouseEvent>
#include "DraggableScrollArea.h"

DraggableScrollArea::DraggableScrollArea(QWidget * parent):
	QAbstractScrollArea(parent)
{
	setMouseTracking(true);
	viewport()->setCursor(Qt::OpenHandCursor);
}

void DraggableScrollArea::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_dragging = true;
		m_dragStartPos = event->pos();
		m_hScrollStart = horizontalScrollBar()->value();
		m_vScrollStart = verticalScrollBar()->value();
		viewport()->setCursor(Qt::ClosedHandCursor);
		event->accept();
	} else {
		QAbstractScrollArea::mousePressEvent(event);
	}
}

void DraggableScrollArea::mouseMoveEvent(QMouseEvent *event)
{
	if (m_dragging) {
		const QPoint delta = event->pos() - m_dragStartPos;
		horizontalScrollBar()->setValue(m_hScrollStart - delta.x());
		verticalScrollBar()->setValue(m_vScrollStart - delta.y());
		event->accept();
	} else {
		QAbstractScrollArea::mouseMoveEvent(event);
	}
}

void DraggableScrollArea::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_dragging && event->button() == Qt::LeftButton) {
		m_dragging = false;
		viewport()->setCursor(Qt::OpenHandCursor);
		event->accept();
	} else {
		QAbstractScrollArea::mouseReleaseEvent(event);
	}
}
