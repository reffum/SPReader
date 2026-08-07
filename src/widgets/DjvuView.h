//
// DjvuView widget for rendering and navigating DjVu document pages
//
#ifndef SPREADER_DJVUVIEW_H
#define SPREADER_DJVUVIEW_H

#include <QScrollArea>
#include <QLabel>
#include <QVBoxLayout>
#include <QFutureWatcher>
#include "data/DjvuDocument.h"

class DjvuView : public QScrollArea
{
	Q_OBJECT

	DjvuDocument * m_document{nullptr};
	QWidget * m_containerWidget{nullptr};
	QVBoxLayout * m_layout{nullptr};
	QList<QLabel *> m_pageLabels;

	int m_currentPage{0};
	double m_zoomFactor{1.0};

	void clearPages();
	void renderPages();
	void updateCurrentPageFromScroll();

protected:
	void scrollContentsBy(
		int dx,
		int dy
	) override;

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
};

#endif // SPREADER_DJVUVIEW_H
