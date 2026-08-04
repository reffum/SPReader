//
// Created by oleg on 8/4/26.
//

#include "ContentsTreeView.h"

#include <QHeaderView>

#include "models/PdfContentsModel.h"

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

	PdfContentsModel * m = qobject_cast<PdfContentsModel*>(model());

	int pageNumber = m->getPageNumber(index);
	emit navigateToPage(pageNumber);
}
