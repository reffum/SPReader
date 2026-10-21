//
// Created by oleg on 10/21/26.
//

#ifndef SPREADER_DJVUWIDGET_H
#define SPREADER_DJVUWIDGET_H
#include "DocumentWidget.h"

class DjvuView;
class DjvuContentsModel;

class DjvuWidget: public DocumentWidget
{
	Q_OBJECT
public:
	explicit DjvuWidget(const QString& fileName, QWidget* parent = nullptr);
	~DjvuWidget() override;

	[[nodiscard]] int getCurrentPage() const override;
	[[nodiscard]] int getPageCount() const override;

	[[nodiscard]] qreal getZoomFactor() const override;

	void setZoomFactor(qreal zoom) override;
	void setCurrentPage(int page) override;

	[[nodiscard]] QAbstractItemModel * contentsModel() const override;
	[[nodiscard]] QAbstractItemModel * bookmarksModel() const override;

private:
	DjvuView * m_djvuView;
	DjvuContentsModel * m_djvuContentsModel;

};


#endif //SPREADER_DJVUWIDGET_H
