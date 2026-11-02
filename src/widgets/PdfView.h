//
// Created by oleg on 9/12/26.
//

#ifndef SPREADER_PDFVIEW_H
#define SPREADER_PDFVIEW_H

#include <QImage>
#include <QPdfView>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>

class QContextMenuEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

class PdfView : public QPdfView
{
	Q_OBJECT
public:
	enum class Mode
	{
		Hand,
		SelectText,
		SelectImage
	};

	explicit PdfView(QWidget *parent = nullptr);

	void setMode(Mode mode);
	[[nodiscard]] Mode mode() const;

	void clearSelection();
	void copySelectionToClipboard() const;

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void contextMenuEvent(QContextMenuEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

private:
	struct PageGeometry
	{
		QRect rectInDoc;
		qreal scale{0.0};
	};

	[[nodiscard]] qreal screenResolution() const;
	[[nodiscard]] qreal scaleForPage(int page) const;
	[[nodiscard]] PageGeometry pageGeometry(int targetPage) const;

	bool mapViewportToPage(
		const QPoint &viewportPos,
		int &outPage,
		QPointF &outPagePt
	) const;

	[[nodiscard]] QPointF mapViewportToSpecificPage(
		const QPoint &viewportPos,
		int page
	) const;

	[[nodiscard]] QImage renderSelectedImage() const;

	Mode m_mode{Mode::Hand};

	// Document drag-and-drop variables
	bool m_dragging{false};
	QPoint m_dragStartPos;
	int m_hScrollStart{0};
	int m_vScrollStart{0};

	// Document selection variables
	int m_selectedPage{-1};
	QPointF m_startPagePt;
	QPointF m_endPagePt;
	QRectF m_imageRectPts;
};

#endif //SPREADER_PDFVIEW_H
