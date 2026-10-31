//
// Created by oleg on 10/22/26.
//

#include "BookmarksModel.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "constants.h"

const QString DEFAULT_BOOKMARK_NAME = "New bookmark";

struct TreeItem
{
	QString name;
	int page = 0;

	TreeItem* parent;
	QList<TreeItem*> childItems;

	TreeItem(TreeItem* parent=nullptr):
		parent(parent)
	{
		page = 0;
		name = DEFAULT_BOOKMARK_NAME;
	}

	TreeItem(QJsonObject obj, TreeItem* parent=nullptr):
		parent(parent)
	{
		name = obj["name"].toString();
		page = obj["page"].toInt();

		if (obj.contains("children"))
		{
			const QJsonArray childrenArray = obj["children"].toArray();
			for (const QJsonValue value : childrenArray)
			{
				if (value.isObject())
				{
					TreeItem* childItem = new TreeItem(value.toObject(), this);
					childItems.append(childItem);
				}
			}
		}
	}

	~TreeItem()
	{
		qDeleteAll(childItems);
	}

	QJsonObject toJson()
	{
		QJsonObject obj;
		obj["name"] = name;
		obj["page"] = page;

		if (!childItems.isEmpty())
		{
			QJsonArray childrenArray;

			for (TreeItem* childItem : childItems)
			{
				childrenArray.append(childItem->toJson());
			}

			obj["children"] = childrenArray;
		}

		return obj;
	}
};

BookmarksModel::BookmarksModel(const QString &jsonFileName, QObject *parent):
	QAbstractItemModel(parent),
	m_jsonFileName(jsonFileName),
	m_rootItem(new TreeItem()),
	m_currentPage(0)
{
	deserialize();
}

BookmarksModel::~BookmarksModel()
{
	delete m_rootItem;
}

QModelIndex BookmarksModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return {};

	TreeItem * parentItem = parent.isValid() ?
								static_cast<TreeItem*>(parent.internalPointer()) :
								m_rootItem;
	TreeItem *item = parentItem->childItems[row];
	return createIndex(row, column, item);
}

QModelIndex BookmarksModel::parent(const QModelIndex &child) const
{
	if (child.isValid())
	{
		TreeItem * childItem = static_cast<TreeItem*>(child.internalPointer());
		TreeItem * parentItem = childItem->parent;

		if (!parentItem)
		{
			return {};
		}

		TreeItem * pParentItem = parentItem->parent;
		int parentRow = pParentItem ? static_cast<int>(pParentItem->childItems.indexOf(parentItem)) : 0;
		return createIndex(parentRow, 0, parentItem);
	}
	return {};
}

int BookmarksModel::rowCount(const QModelIndex &parent) const
{
	TreeItem * parentItem = parent.isValid() ?
								static_cast<TreeItem*>(parent.internalPointer()):
								m_rootItem;
	return static_cast<int>(parentItem->childItems.count());
}

int BookmarksModel::columnCount(const QModelIndex &parent) const
{
	return COLUMN_COUNT;
}

QVariant BookmarksModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return {};

	int column = index.column();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		TreeItem* item = static_cast<TreeItem *>(index.internalPointer());
		switch (column)
		{
			case COLUMN_TITLE:
				return item->name;
			case COLUMN_PAGE:
				return item->page;
			default: ;
		}
	}
	return {};
}

bool BookmarksModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
	int column = index.column();

	if (role == Qt::EditRole)
	{
		switch (column)
		{
			case COLUMN_TITLE:
				item->name = value.toString();
				break;
			case COLUMN_PAGE:
				item->page = value.toInt();
				break;
			default:
				break;
		}

		emit dataChanged(index, index, {Qt::DisplayRole});
		serialize();

		return true;
	}

	return false;
}

QVariant BookmarksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			switch (section)
			{
				case COLUMN_TITLE:
					return COLUMN_TITLE_HEADER;
				case COLUMN_PAGE:
					return COLUMN_PAGE_HEADER;
				default: ;
			}
		}
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags BookmarksModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	if (index.column() == COLUMN_TITLE)
		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	return QAbstractItemModel::flags(index);
}

bool BookmarksModel::insertRows(int row, int count, const QModelIndex &parent)
{
	TreeItem * parentItem = parent.isValid() ?
								static_cast<TreeItem*>(parent.internalPointer()) :
								m_rootItem;

	beginInsertRows(parent, row, row + count - 1);

	for (int i = 0; i < count; i++)
	{
		TreeItem * newItem = new TreeItem(parentItem);
		newItem->page = m_currentPage;

		parentItem->childItems.insert(row + i, newItem);
	}

	endInsertRows();

	serialize();

	return true;
}

bool BookmarksModel::removeRows(int row, int count, const QModelIndex& parent)
{
	TreeItem * parentItem = parent.isValid() ?
								static_cast<TreeItem*>(parent.internalPointer()) :
								m_rootItem;

	assert(parentItem->childItems.count() >= row + count);

	beginRemoveRows(parent, row, row + count - 1);
	parentItem->childItems.remove(row, count);
	endRemoveRows();

	serialize();

	return true;
}

void BookmarksModel::setCurrentPage(int page)
{
	m_currentPage = page;
}

// Serialize a bookmarks in JSON file m_jsonFileName
void BookmarksModel::serialize() const
{
	QJsonArray rootArray;
	for (TreeItem * item : m_rootItem->childItems)
	{
		rootArray.append(item->toJson());
	}

	QJsonDocument doc(rootArray);
	QFile file(m_jsonFileName);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qWarning() << "Failed to open file for writing:" << m_jsonFileName;
		throw BookmarksReadError("Failed to open file for writing:");
	}

	file.write(doc.toJson(QJsonDocument::Indented));
	file.close();
}

void BookmarksModel::deserialize()
{
	QFile file(m_jsonFileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}

	const QByteArray jsonData = file.readAll();
	file.close();

	QJsonParseError parseError;
	const QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

	if (parseError.error != QJsonParseError::NoError)
	{
		qWarning() << "JSON parse error in" << m_jsonFileName
			<< ":" << parseError.errorString();
		throw BookmarksReadError("Failed to parse JSON file");
	}

	if (!doc.isArray() && !doc.isObject())
	{
		qWarning() << "Invalid JSON root in" << m_jsonFileName;
		throw BookmarksReadError("Invalid JSON root in");
	}

	beginResetModel();

	const QJsonArray rootArray = doc.array();
	for (QJsonValue value : rootArray)
	{
		if (value.isObject())
		{
			QJsonObject object = value.toObject();
			TreeItem * item = new TreeItem(object);

			m_rootItem->childItems.append(item);
		}
	}

	endResetModel();
}
