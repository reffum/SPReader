//
// Created by oleg on 10/22/26.
//

#ifndef SPREADER_BOOKMARKSTREEVIEW_H
#define SPREADER_BOOKMARKSTREEVIEW_H

#include <QTreeView>


class BookmarksTreeView : public QTreeView
{
	Q_OBJECT
public:
	explicit BookmarksTreeView(QWidget* parent = nullptr);

	void setModel(QAbstractItemModel *model) override;

protected:
	void contextMenuEvent(QContextMenuEvent *event) override;
};


#endif //SPREADER_BOOKMARKSTREEVIEW_H
