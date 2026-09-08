//
// DraggableScrollArea widget for drag‑scrolling content via left‑button mouse drag
//
#ifndef SPREADER_DRAGGABLESCROLLAREA_H
#define SPREADER_DRAGGABLESCROLLAREA_H

#include <QScrollArea>
#include <QMouseEvent>
#include <QPoint>

class DraggableScrollArea : public QAbstractScrollArea
{
	Q_OBJECT

public:
	explicit DraggableScrollArea(QWidget * parent = nullptr);

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

private:
	bool m_dragging{false};
	QPoint m_dragStartPos;
	int m_hScrollStart{0};
	int m_vScrollStart{0};
};

#endif // SPREADER_DRAGGABLESCROLLAREA_H
