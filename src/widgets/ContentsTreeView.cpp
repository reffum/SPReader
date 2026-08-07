//
// Created by oleg on 8/4/26.
//

#include "ContentsTreeView.h"

#include <QHeaderView>

#include "models/PdfContentsModel.h"
#include "models/DjvuContentsModel.h"

ContentsTreeView::ContentsTreeView(QWidget *parent):
	QTreeView(parent)
{
	connect(
		this,
		&ContentsTreeView::doubleClicked,
		this,
		&ContentsTreeView::onItemDoubleClicked
	);
}

void ContentsTreeView::setModel(QAbstractItemModel* model)
{
	QTreeView::setModel(model);

	if (model == nullptr)
	{
		return;
	}

	QHeaderView * h = header();
	h->setStretchLastSection(false);
	h->setSectionResizeMode(PdfContentsModel::COLUMN_TITLE, QHeaderView::Stretch);
	resizeColumnToContents(PdfContentsModel::COLUMN_PAGE);
}

void ContentsTreeView::onItemDoubleClicked(const QModelIndex &index)
{
	if (!index.isValid())
	{
		return;
	}

	int pageNumber = -1;
	if (auto * pdfModel = qobject_cast<PdfContentsModel*>(model()))
	{
		pageNumber = pdfModel->getPageNumber(index);
	}
	else if (auto * djvuModel = qobject_cast<DjvuContentsModel*>(model()))
	{
		pageNumber = djvuModel->getPageNumber(index);
	}

	if (pageNumber >= 0)
	{
		emit navigateToPage(pageNumber);
	}
}
