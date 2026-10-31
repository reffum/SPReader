//
// Created by oleg on 10/15/26.
//

#ifndef SPREADER_DOCUMENTWIDGET_H
#define SPREADER_DOCUMENTWIDGET_H
#include <QAbstractItemModel>
#include <qwidget.h>

class BookmarksModel;

class OpenDocumentError : public std::runtime_error
{
public:
	OpenDocumentError(const QString &message) : std::runtime_error(message.toStdString()) {}
};

//
// Widget display a document.
// This is base class of all document types
//

class DocumentWidget: public QWidget
{
	Q_OBJECT

public:
	static DocumentWidget* openDocument(const QString &fileName);

	[[nodiscard]] virtual int getCurrentPage() const = 0;
	[[nodiscard]] virtual int getPageCount() const = 0;
	[[nodiscard]] virtual qreal getZoomFactor() const = 0;

	virtual void setZoomFactor(qreal zoom) = 0;
	virtual void setCurrentPage(int page);

	[[nodiscard]] virtual QAbstractItemModel * contentsModel() const = 0;

	[[nodiscard]] QAbstractItemModel * bookmarksModel() const;
	[[nodiscard]] QString getFileName() const;
	[[nodiscard]] QString getFullFileName() const;

signals:
	void currentPageChanged(int page);
	void zoomChanged(qreal zoom);

protected:
	DocumentWidget(const QString& fileName, QWidget* parent=nullptr);

	QString m_fullFileName;
	BookmarksModel* m_bookmarksModel;

private slots:
	void on_currentPageChanged(int page) const;
};

#endif //SPREADER_DOCUMENTWIDGET_H
