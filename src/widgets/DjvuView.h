//
// DjvuView widget for rendering and navigating DjVu document pages
//
#ifndef SPREADER_DJVUVIEW_H
#define SPREADER_DJVUVIEW_H

#include <QAbstractScrollArea>
#include <optional>
#include <utility>

#include "data/DjvuDocument.h"


class DjvuView : public QAbstractScrollArea
{
	Q_OBJECT

	//
	// Information about one page
	//
	struct Page
	{
		QRect rect;
		int number{0};
		QImage image;
	};

	DjvuDocument * m_document{nullptr};

	int m_currentPage{0};
	double m_zoomFactor{1.0};

	QList<Page> pages;

public:
	explicit DjvuView(QWidget * parent = nullptr);

	void setDocument(DjvuDocument * document);
	[[nodiscard]] DjvuDocument * document() const;

	[[nodiscard]] int currentPage() const;
	void setCurrentPage(int pageNumber);

	[[nodiscard]] double zoomFactor() const;
	void setZoomFactor(double factor);


signals:
	void currentPageChanged(int pageNumber);

protected:
	void resizeEvent(QResizeEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

private:
	// Total size of the whole view
	int totalHeight{0};
	int totalWidth{0};

	// Computes the part of the page rect that is visible in the viewport.
	// Returns an optional pair: the visible rect in the page's own coordinate
	// system and its position in the viewport coordinate system.
	// Contains no value if the page is not visible at all.
	static std::optional<std::pair<QRect, QPoint>> visiblePageRect(
		const QRect & viewportRect,
		const QRect & pageRect) ;

	// Returns the currently visible viewport rect in document coordinates
	[[nodiscard]] QRect getViewportRect() const;

	void recreatePages();
	void rearrangePages();
	void setupScrollBars() const;
	void renderPage(QPainter& painter, Page& page, QRect pageRect, QPoint pageOffset) const;
	void renderPages();
};

#endif // SPREADER_DJVUVIEW_H
