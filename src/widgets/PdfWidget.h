//
// Created by oleg on 10/18/26.
//

#ifndef SPREADER_PDFWIDGET_H
#define SPREADER_PDFWIDGET_H

#include "DocumentWidget.h"
#include "PdfView.h"
#include "models/PdfContentsModel.h"

class PdfWidget: public DocumentWidget
{
	Q_OBJECT
public:
	explicit PdfWidget(const QString& fileName, QWidget* parent = nullptr);
	~PdfWidget() override;

	void setMode(Mode mode) override;
	Mode getMode() override;

	[[nodiscard]] int getCurrentPage() const override;
	[[nodiscard]] int getPageCount() const override;

	[[nodiscard]] qreal getZoomFactor() const override;

	void setZoomFactor(qreal zoom) override;
	void setCurrentPage(int page) override;

	[[nodiscard]] QAbstractItemModel * contentsModel() const override;

	void setMode(PdfView::Mode mode);
	[[nodiscard]] PdfView::Mode mode() const;
	void copySelectionToClipboard() const;
	[[nodiscard]] PdfView * pdfView() const;

private:
	PdfView * m_pdfView;
	PdfContentsModel * m_pdfContentsModel;
};

#endif //SPREADER_PDFWIDGET_H
