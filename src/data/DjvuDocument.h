//
// DjvuDocument wrapping libdjvulibre ddjvu API
//
#ifndef SPREADER_DJVUDOCUMENT_H
#define SPREADER_DJVUDOCUMENT_H

#include <QImage>
#include <QList>
#include <QFuture>
#include <libdjvu/ddjvuapi.h>

struct DjvuOutlineItem
{
	QString title;
	int pageNumber{0};
	QList<DjvuOutlineItem> children;
};

class DjvuDocument : public QObject
{
	Q_OBJECT

	ddjvu_context_t * m_context{nullptr};
	ddjvu_document_t * m_document{nullptr};
	int m_pageCount{0};

	void handleEvents() const;
	void parseOutlineExpression(
		miniexp_t expr,
		QList<DjvuOutlineItem> & items
	);

public:
	explicit DjvuDocument(QObject * parent = nullptr);
	~DjvuDocument() override;

	bool load(const QString & fileName);
	[[nodiscard]] int pageCount() const;
	[[nodiscard]] QFuture<QImage> renderPage(
		int pageNumber,
		double zoomFactor = 1.0
	) const;
	[[nodiscard]] QList<DjvuOutlineItem> outline();
};

#endif // SPREADER_DJVUDOCUMENT_H
