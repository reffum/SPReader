#include <QApplication>

#include "models/BookmarksModel.h"
#include "widgets/BookmarksTreeView.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QString jsonFilePath = argc > 1 ? argv[1] : "bookmarks.json";

	BookmarksTreeView treeView;
	BookmarksModel model(jsonFilePath);
	treeView.setModel(&model);

	model.setCurrentPage(100);

	treeView.setWindowTitle("JSON Bookmarks Viewer");
	treeView.resize(600, 400);
	treeView.show();

	return app.exec();
}
