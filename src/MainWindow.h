//
// Created by oleg on 7/20/26.
//

#ifndef SPREADER_MAINWINDOW_H
#define SPREADER_MAINWINDOW_H
#include <qt/QtWidgets/QMainWindow>

#include "ui/ui_mainwindow.h"

class QPdfDocument;
class QPdfView;
class QSpinBox;
class QDoubleSpinBox;

class MainWindow : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT

	QSpinBox * m_pageSpinBox;
	QDoubleSpinBox * m_zoomSpinBox;

	QPdfView * currentPdfView() const;
	QPdfDocument * currentPdfDocument() const;
public:
	explicit MainWindow(QWidget* parent = nullptr);
private slots:
	void on_action_Open_triggered(bool checked = false);
	void on_actionAbout_triggered();
	void on_contents_treeView_navigateToPage(int pageNumber) const;
	void on_actionNext_page_triggered(bool checked = false) const;
	void on_actionPrev_page_triggered(bool checked = false) const;
	void on_m_pageSpinBox_valueChanged(int value) const;
	void on_actionZoom_In_triggered(bool checked = false) const;
	void on_actionZoom_Out_triggered(bool checked = false) const;
	void on_m_zoomSpinBox_valueChanged(double) const;

	void pageNavigator_currentPageChanged(int value) const;
	void on_centralwidget_currentChanged(int index);
	void on_centralwidget_tabCloseRequested(int index);
protected:
	void resizeEvent(QResizeEvent * event) override;
};

#endif //SPREADER_MAINWINDOW_H
