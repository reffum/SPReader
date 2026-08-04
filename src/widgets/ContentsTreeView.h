//
// Created by oleg on 8/4/26.
//

#ifndef SPREADER_CONTENTSTREEWIDGET_H
#define SPREADER_CONTENTSTREEWIDGET_H
#include <QTreeView>


class ContentsTreeView: public QTreeView
{
	Q_OBJECT
public:
	explicit ContentsTreeView(QWidget* parent = nullptr);
	void setModel(QAbstractItemModel* model) override;

signals:
	void navigateToPage(int pageNumber);

private slots:
	void onItemDoubleClicked(const QModelIndex &index);
};


#endif //SPREADER_CONTENTSTREEWIDGET_H
