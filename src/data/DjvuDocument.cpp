//
// DjvuDocument implementation
//
#include "DjvuDocument.h"
#include <QDebug>
#include <QPromise>
#include <QtConcurrent>

DjvuDocument::DjvuDocument(QObject * parent):
	QObject(parent)
{
	m_context = ddjvu_context_create("SPReader");
}

DjvuDocument::~DjvuDocument()
{
	if (m_document != nullptr)
	{
		ddjvu_document_release(m_document);
		m_document = nullptr;
	}
	if (m_context != nullptr)
	{
		ddjvu_context_release(m_context);
		m_context = nullptr;
	}
}

void DjvuDocument::handleEvents() const
{
	if (m_context == nullptr)
	{
		return;
	}

	while (ddjvu_message_peek(m_context))
	{
		ddjvu_message_pop(m_context);
	}
}

bool DjvuDocument::load(const QString & fileName)
{
	if (m_context == nullptr)
	{
		return false;
	}

	if (m_document != nullptr)
	{
		ddjvu_document_release(m_document);
		m_document = nullptr;
	}

	m_document = ddjvu_document_create_by_filename(
		m_context,
		fileName.toUtf8().constData(),
		TRUE
	);

	if (m_document == nullptr)
	{
		return false;
	}

	while (!ddjvu_document_decoding_done(m_document))
	{
		handleEvents();
	}

	if (ddjvu_document_decoding_error(m_document))
	{
		qWarning() << "DjVu document decoding failed:" << fileName;
		return false;
	}

	m_pageCount = ddjvu_document_get_pagenum(m_document);
	return true;
}

int DjvuDocument::pageCount() const
{
	return m_pageCount;
}

QFuture<QImage> DjvuDocument::renderPage(
	int pageNumber,
	double zoomFactor
) const
{
	return QtConcurrent::run(
		[this, pageNumber, zoomFactor](QPromise<QImage> & promise)
		{
			if (m_document == nullptr || pageNumber < 0 || pageNumber >= m_pageCount)
			{
				promise.addResult(QImage());
				return;
			}

			ddjvu_page_t * page = ddjvu_page_create_by_pageno(
				m_document,
				pageNumber
			);

			if (page == nullptr)
			{
				promise.addResult(QImage());
				return;
			}

			while (!ddjvu_page_decoding_done(page))
			{
				if (promise.isCanceled())
				{
					ddjvu_page_release(page);
					return;
				}
				const_cast<DjvuDocument *>(this)->handleEvents();
			}

			if (ddjvu_page_decoding_error(page) || promise.isCanceled())
			{
				ddjvu_page_release(page);
				promise.addResult(QImage());
				return;
			}

			int width = ddjvu_page_get_width(page);
			int height = ddjvu_page_get_height(page);

			int targetWidth = static_cast<int>(width * zoomFactor);
			int targetHeight = static_cast<int>(height * zoomFactor);

			ddjvu_rect_t pageRect;
			pageRect.x = 0;
			pageRect.y = 0;
			pageRect.w = targetWidth;
			pageRect.h = targetHeight;

			ddjvu_rect_t renderRect = pageRect;

			ddjvu_format_t * format = ddjvu_format_create(
				DDJVU_FORMAT_RGB24,
				0,
				nullptr
			);
			ddjvu_format_set_row_order(format, 1);

			QImage img(targetWidth, targetHeight, QImage::Format_RGB888);

			ddjvu_page_render(
				page,
				DDJVU_RENDER_COLOR,
				&pageRect,
				&renderRect,
				format,
				img.bytesPerLine(),
				reinterpret_cast<char *>(img.bits())
			);

			ddjvu_format_release(format);
			ddjvu_page_release(page);

			if (!promise.isCanceled())
			{
				promise.addResult(img);
			}
		}
	);
}

void DjvuDocument::parseOutlineExpression(
	miniexp_t expr,
	QList<DjvuOutlineItem> & items
)
{
	if (!miniexp_consp(expr))
	{
		return;
	}

	miniexp_t cur = expr;
	while (miniexp_consp(cur))
	{
		miniexp_t entry = miniexp_car(cur);
		cur = miniexp_cdr(cur);

		if (!miniexp_consp(entry))
		{
			continue;
		}

		miniexp_t titleExpr = miniexp_car(entry);
		miniexp_t rest = miniexp_cdr(entry);

		if (!miniexp_stringp(titleExpr) || !miniexp_consp(rest))
		{
			continue;
		}

		QString title = QString::fromUtf8(miniexp_to_str(titleExpr));
		miniexp_t destExpr = miniexp_car(rest);

		int pageNum = -1;
		if (miniexp_stringp(destExpr))
		{
			const char * destStr = miniexp_to_str(destExpr);
			if (destStr != nullptr && destStr[0] == '#')
			{
				pageNum = ddjvu_document_search_pageno(
					m_document,
					destStr + 1
				);
			}
			else if (destStr != nullptr)
			{
				pageNum = ddjvu_document_search_pageno(
					m_document,
					destStr
				);
			}
		}
		else if (miniexp_numberp(destExpr))
		{
			pageNum = miniexp_to_int(destExpr);
		}

		DjvuOutlineItem item;
		item.title = title;
		item.pageNumber = pageNum;

		miniexp_t childrenExpr = miniexp_cdr(rest);
		if (miniexp_consp(childrenExpr))
		{
			parseOutlineExpression(childrenExpr, item.children);
		}

		items.append(item);
	}
}

QList<DjvuOutlineItem> DjvuDocument::outline()
{
	QList<DjvuOutlineItem> result;
	if (m_document == nullptr)
	{
		return result;
	}

	miniexp_t outlineExp = miniexp_nil;
	while ((outlineExp = ddjvu_document_get_outline(m_document)) == miniexp_dummy)
	{
		handleEvents();
	}

	if (outlineExp != miniexp_nil && outlineExp != miniexp_dummy)
	{
		parseOutlineExpression(outlineExp, result);
		ddjvu_miniexp_release(m_document, outlineExp);
	}

	return result;
}
