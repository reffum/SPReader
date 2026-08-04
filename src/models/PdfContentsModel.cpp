//
// Created by oleg on 8/2/26.
//

#include "PdfContentsModel.h"

const QList<QString> ContentsHeaders = {"Title", "Page"};

PdfContentsModel::PdfContentsModel(QObject *parent)
	:QPdfBookmarkModel(parent)
{
}

int PdfContentsModel::columnCount(const QModelIndex &parent) const
{
	return COLUMN_COUNT;
}

QVariant PdfContentsModel::headerData(
	int section,
	Qt::Orientation orientation,
	int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		return ContentsHeaders[section];
	}
	return {};
}

QVariant PdfContentsModel::data(
	const QModelIndex &index,
	int role) const
{
	if (role == Qt::DisplayRole)
	{
		int column = index.column();
		if (column == COLUMN_TITLE)
		{
			return QPdfBookmarkModel::data(
				index,
				static_cast<int>(QPdfBookmarkModel::Role::Title)
			);
		}
		else if (column == COLUMN_PAGE)
		{
			return QPdfBookmarkModel::data(
				index,
				static_cast<int>(QPdfBookmarkModel::Role::Page)
			);
		}
	}
	return QPdfBookmarkModel::data(index, role);
}

int PdfContentsModel::getPageNumber(const QModelIndex & index) const
{
	QVariant value = QPdfBookmarkModel::data(
		index,
		static_cast<int>(QPdfBookmarkModel::Role::Page)
	);
	return value.toInt();
}
