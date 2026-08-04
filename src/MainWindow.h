//
// Created by oleg on 7/20/26.
//

#ifndef SPREADER_MAINWINDOW_H
#define SPREADER_MAINWINDOW_H
#include <qt/QtWidgets/QMainWindow>

#include "ui/ui_mainwindow.h"

class QPdfDocument;
class QSpinBox;

class MainWindow : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT

	QPdfDocument * m_pdfDocument;
	QSpinBox * m_pageSpinBox;
public:
	explicit MainWindow(QWidget* parent = nullptr);
private slots:
	void on_action_Open_triggered(bool checked = false);
	void on_contents_treeView_navigateToPage(int pageNumber);
	void on_actionNext_page_triggered(bool checked = false) const;
	void on_actionPrev_page_triggered(bool checked = false) const;
	void on_m_pageSpinBox_valueChanged(int value) const;
	void on_pageNavigator_currentPageChanged(int value) const;
};

#endif //SPREADER_MAINWINDOW_H
