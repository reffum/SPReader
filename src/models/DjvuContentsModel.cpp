//
// DjvuContentsModel implementation
//
#include "DjvuContentsModel.h"

DjvuContentsNode::DjvuContentsNode(
	DjvuOutlineItem item,
	DjvuContentsNode * parent
):
	item(std::move(item)),
	parentNode(parent)
{
}

DjvuContentsNode::~DjvuContentsNode()
{
	qDeleteAll(children);
}

void DjvuContentsNode::appendChild(DjvuContentsNode * child)
{
	children.append(child);
}

DjvuContentsNode * DjvuContentsNode::child(int row) const
{
	if (row < 0 || row >= children.size())
	{
		return nullptr;
	}
	return children.at(row);
}

int DjvuContentsNode::childCount() const
{
	return children.size();
}

int DjvuContentsNode::row() const
{
	if (parentNode != nullptr)
	{
		return parentNode->children.indexOf(const_cast<DjvuContentsNode *>(this));
	}
	return 0;
}

DjvuContentsModel::DjvuContentsModel(QObject * parent):
	QAbstractItemModel(parent)
{
	m_rootNode = new DjvuContentsNode({});
}

DjvuContentsModel::~DjvuContentsModel()
{
	delete m_rootNode;
}

void DjvuContentsModel::buildTree(
	const QList<DjvuOutlineItem> & items,
	DjvuContentsNode * parent
)
{
	for (const auto & item : items)
	{
		auto * node = new DjvuContentsNode(item, parent);
		parent->appendChild(node);
		if (!item.children.isEmpty())
		{
			buildTree(item.children, node);
		}
	}
}

void DjvuContentsModel::setDocument(DjvuDocument * document)
{
	beginResetModel();
	delete m_rootNode;
	m_rootNode = new DjvuContentsNode({});

	if (document != nullptr)
	{
		QList<DjvuOutlineItem> outline = document->outline();
		buildTree(outline, m_rootNode);
	}
	endResetModel();
}

QModelIndex DjvuContentsModel::index(
	int row,
	int column,
	const QModelIndex & parent
) const
{
	if (!hasIndex(row, column, parent))
	{
		return {};
	}

	DjvuContentsNode * parentNode;
	if (!parent.isValid())
	{
		parentNode = m_rootNode;
	}
	else
	{
		parentNode = static_cast<DjvuContentsNode *>(parent.internalPointer());
	}

	DjvuContentsNode * childNode = parentNode->child(row);
	if (childNode != nullptr)
	{
		return createIndex(row, column, childNode);
	}
	return {};
}

QModelIndex DjvuContentsModel::parent(const QModelIndex & index) const
{
	if (!index.isValid())
	{
		return {};
	}

	auto * childNode = static_cast<DjvuContentsNode *>(index.internalPointer());
	DjvuContentsNode * parentNode = childNode->parentNode;

	if (parentNode == m_rootNode || parentNode == nullptr)
	{
		return {};
	}

	return createIndex(parentNode->row(), 0, parentNode);
}

int DjvuContentsModel::rowCount(const QModelIndex & parent) const
{
	DjvuContentsNode * parentNode;
	if (parent.column() > 0)
	{
		return 0;
	}

	if (!parent.isValid())
	{
		parentNode = m_rootNode;
	}
	else
	{
		parentNode = static_cast<DjvuContentsNode *>(parent.internalPointer());
	}

	return parentNode->childCount();
}

int DjvuContentsModel::columnCount(const QModelIndex & parent) const
{
	return COLUMN_COUNT;
}

QVariant DjvuContentsModel::data(
	const QModelIndex & index,
	int role
) const
{
	if (!index.isValid())
	{
		return {};
	}

	if (role != Qt::DisplayRole)
	{
		return {};
	}

	auto * node = static_cast<DjvuContentsNode *>(index.internalPointer());

	if (index.column() == COLUMN_TITLE)
	{
		return node->item.title;
	}
	if (index.column() == COLUMN_PAGE)
	{
		if (node->item.pageNumber >= 0)
		{
			return node->item.pageNumber;
		}
		return {};
	}

	return {};
}

QVariant DjvuContentsModel::headerData(
	int section,
	Qt::Orientation orientation,
	int role
) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section == COLUMN_TITLE)
		{
			return tr("Title");
		}
		if (section == COLUMN_PAGE)
		{
			return tr("Page");
		}
	}
	return {};
}

int DjvuContentsModel::getPageNumber(const QModelIndex & index) const
{
	if (!index.isValid())
	{
		return -1;
	}
	auto * node = static_cast<DjvuContentsNode *>(index.internalPointer());
	return node->item.pageNumber;
}
