//
// Created by oleg on 7/20/26.
//

#ifndef SPREADER_MAINWINDOW_H
#define SPREADER_MAINWINDOW_H
#include <qt/QtWidgets/QMainWindow>

#include "ui/ui_mainwindow.h"

class QPdfDocument;
class PdfView;
class DjvuDocument;
class DjvuView;
class QSpinBox;
class QDoubleSpinBox;

class MainWindow :
	public QMainWindow,
	Ui::MainWindow
{
	Q_OBJECT

	QSpinBox * m_pageSpinBox;
	QDoubleSpinBox * m_zoomSpinBox;

	void updateRecentFilesMenu();
	void addRecentFile(const QString & fileName);
	void setCurrentDocumentPage(int pageNumber) const;
public:
	explicit MainWindow(QWidget * parent = nullptr);

	void openFile(const QString & fileName);
private slots:
	//
	// Auto-connected slots
	//
	void on_action_Open_triggered(bool checked = false);
	void on_actionAbout_triggered(bool checked = false);
	void on_centralTab_currentChanged(int index) const;
	void on_contents_treeView_navigateToPage(int pageNumber) const;
	void on_actionNext_page_triggered(bool checked = false) const;
	void on_actionPrev_page_triggered(bool checked = false) const;
	void on_actionZoom_In_triggered(bool checked = false) const;
	void on_actionZoom_Out_triggered(bool checked = false) const;
	void on_centralTab_tabCloseRequested(int index) const;
	void on_actionContents_triggered(bool checked = false) const;

	//
	// Manually connected slots
	//
	void currentDocument_currentPageChanged(int value) const;
	void currentDocument_zoomChanged(qreal value) const;
	void m_pageSpinBox_valueChanged(int value) const;
	void m_zoomSpinBox_valueChanged(double) const;
	void openRecentFile_action_triggered(bool checked = false);

protected:
	void resizeEvent(QResizeEvent * event) override;
};

#endif //SPREADER_MAINWINDOW_H
