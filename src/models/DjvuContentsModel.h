//
// DjvuContentsModel representation of DjVu document outline
//
#ifndef SPREADER_DJVUCONTENTSMODEL_H
#define SPREADER_DJVUCONTENTSMODEL_H

#include <QAbstractItemModel>
#include "data/DjvuDocument.h"

class DjvuContentsNode
{
public:
	DjvuOutlineItem item;
	DjvuContentsNode * parentNode{nullptr};
	QList<DjvuContentsNode *> children;

	explicit DjvuContentsNode(
		DjvuOutlineItem item,
		DjvuContentsNode * parent = nullptr
	);
	~DjvuContentsNode();

	void appendChild(DjvuContentsNode * child);
	DjvuContentsNode * child(int row) const;
	int childCount() const;
	int row() const;
};

class DjvuContentsModel : public QAbstractItemModel
{
	Q_OBJECT

	DjvuContentsNode * m_rootNode{nullptr};

	void buildTree(
		const QList<DjvuOutlineItem> & items,
		DjvuContentsNode * parent
	);

public:
	static constexpr int COLUMN_COUNT = 2;
	static constexpr int COLUMN_TITLE = 0;
	static constexpr int COLUMN_PAGE = 1;

	explicit DjvuContentsModel(QObject * parent = nullptr);
	~DjvuContentsModel() override;

	void setDocument(DjvuDocument * document);

	[[nodiscard]] QModelIndex index(
		int row,
		int column,
		const QModelIndex & parent = QModelIndex()
	) const override;

	[[nodiscard]] QModelIndex parent(const QModelIndex & index) const override;

	[[nodiscard]] int rowCount(const QModelIndex & parent = QModelIndex()) const override;

	[[nodiscard]] int columnCount(const QModelIndex & parent = QModelIndex()) const override;

	[[nodiscard]] QVariant data(
		const QModelIndex & index,
		int role = Qt::DisplayRole
	) const override;

	[[nodiscard]] QVariant headerData(
		int section,
		Qt::Orientation orientation,
		int role = Qt::DisplayRole
	) const override;

	[[nodiscard]] int getPageNumber(const QModelIndex & index) const;
};

#endif // SPREADER_DJVUCONTENTSMODEL_H
