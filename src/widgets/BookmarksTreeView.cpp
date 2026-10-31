//
// Created by oleg on 10/22/26.
//
#include <QMenu>
#include <qevent.h>

#include "constants.h"
#include "BookmarksTreeView.h"

#include <QHeaderView>


BookmarksTreeView::BookmarksTreeView(QWidget *parent):
	QTreeView(parent)
{

}

void BookmarksTreeView::setModel(QAbstractItemModel *model)
{
	QTreeView::setModel(model);

	// Stretch Name column
	// Fit to size Page column
	QHeaderView * header = this->header();
	header->setStretchLastSection(false);
	header->setSectionResizeMode(COLUMN_TITLE, QHeaderView::Stretch);
	header->setSectionResizeMode(COLUMN_PAGE, QHeaderView::ResizeToContents);
}

void BookmarksTreeView::contextMenuEvent(QContextMenuEvent *event)
{
	QModelIndex index = indexAt(event->pos());
	QMenu menu(this);

	QAction * addAction = menu.addAction("Add Bookmark");
	QAction * removeAction = menu.addAction("Remove Bookmark");

	if (!model())
	{
		addAction->setEnabled(false);
		removeAction->setEnabled(false);
	}
	else
	{
		removeAction->setEnabled(index.isValid());
	}

	QAction * selectedAction = menu.exec(event->globalPos());

	if (selectedAction == addAction)
	{
		int row = model()->rowCount(index);
		model()->insertRow(row, index);
	}
	else if (selectedAction == removeAction)
	{
		QModelIndex parentIndex = index.parent();
		int row = index.row();
		model()->removeRow(row, parentIndex);
	}

	QTreeView::contextMenuEvent(event);
}
