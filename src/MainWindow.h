//
// Created by oleg on 7/20/26.
//

#ifndef SPREADER_MAINWINDOW_H
#define SPREADER_MAINWINDOW_H
#include <qt/QtWidgets/QMainWindow>

#include "ui/ui_mainwindow.h"

class QPdfDocument;

class MainWindow : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT

	QPdfDocument * m_pdfDocument;
public:
	explicit MainWindow(QWidget* parent = nullptr);
private slots:
	void on_action_Open_triggered(bool checked = false);
};

#endif //SPREADER_MAINWINDOW_H
