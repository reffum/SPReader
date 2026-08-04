//
// Created by oleg on 8/2/26.
//

#ifndef SPREADER_PDFCONTENTSMODEL_H
#define SPREADER_PDFCONTENTSMODEL_H
#include <QPdfBookmarkModel>

//
// This is QTreeView model for contents of pdf document.
// It returns data in 2 columns: Title and page
//

class PdfContentsModel : public QPdfBookmarkModel
{
	Q_OBJECT
public:
	static constexpr int COLUMN_COUNT = 2;
	static constexpr int COLUMN_TITLE = 0;
	static constexpr int COLUMN_PAGE = 1;

	explicit PdfContentsModel(QObject *parent = nullptr);
	[[nodiscard]] int columnCount(const QModelIndex& parent) const override;

	[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	[[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
};


#endif //SPREADER_PDFCONTENTSMODEL_H
