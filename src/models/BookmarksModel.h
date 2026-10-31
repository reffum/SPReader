//
// Created by oleg on 10/22/26.
//

#ifndef SPREADER_JSONITEMMODEL_H
#define SPREADER_JSONITEMMODEL_H
#include <QAbstractItemModel>

struct TreeItem;

class BookmarksReadError: public std::runtime_error
{
public:
	explicit BookmarksReadError(const QString &errorMessage) : std::runtime_error(errorMessage.toStdString()) {}
};

class BookmarksModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit BookmarksModel(const QString &jsonFileName, QObject *parent = nullptr);
	~BookmarksModel() override;

	[[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	[[nodiscard]] QModelIndex parent(const QModelIndex &child) const override;
	[[nodiscard]] int rowCount(const QModelIndex &parent) const override;
	[[nodiscard]] int columnCount(const QModelIndex &parent) const override;
	[[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	[[nodiscard]] Qt::ItemFlags flags(const QModelIndex &) const override;

	bool insertRows(int row, int count, const QModelIndex &parent) override;
	bool removeRows(int row, int count, const QModelIndex& parent) override;

	void setCurrentPage(int page);

private:
	void serialize() const;
	void deserialize();

	QString m_jsonFileName;
	TreeItem* m_rootItem;
	int m_currentPage;
};


#endif //SPREADER_JSONITEMMODEL_H
