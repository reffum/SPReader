//
// Created by oleg on 9/12/26.
//

#ifndef SPREADER_PDFVIEW_H
#define SPREADER_PDFVIEW_H
#include <QPdfView>
#include <QPoint>

class PdfView : public QPdfView
{
	Q_OBJECT
public:
	explicit PdfView(QWidget* parent);

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


#endif //SPREADER_PDFVIEW_H
