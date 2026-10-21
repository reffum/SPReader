//
// Created by oleg on 8/4/26.
//
#include <QHeaderView>

#include "constants.h"

#include "ContentsTreeView.h"

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
	h->setSectionResizeMode(COLUMN_TITLE, QHeaderView::Stretch);
	resizeColumnToContents(COLUMN_PAGE);
}

void ContentsTreeView::onItemDoubleClicked(const QModelIndex &index)
{
	if (!index.isValid())
	{
		return;
	}

	int pageNumber = model()->data(index, PAGE_NUMBER_DATA_ROLE).toInt();
	emit navigateToPage(pageNumber);
}
