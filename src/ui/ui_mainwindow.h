/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include "widgets/ContentsTreeView.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Open;
    QAction *actionPrev_page;
    QAction *actionNext_page;
    QAction *actionZoom_In;
    QAction *actionZoom_Out;
    QAction *actionAbout;
    QAction *actionHand_Tool;
    QAction *actionContents;
    QAction *actionBookmars;
    QTabWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menuNavigation;
    QMenu *menuView;
    QMenu *menuHelp;
    QMenu *menuTools;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QDockWidget *contents_dockWidget;
    ContentsTreeView *contents_treeView;
    QVBoxLayout *verticalLayout;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icon.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        MainWindow->setWindowIcon(icon);
        action_Open = new QAction(MainWindow);
        action_Open->setObjectName("action_Open");
        QIcon icon1(QIcon::fromTheme(QString::fromUtf8("document-open")));
        action_Open->setIcon(icon1);
        actionPrev_page = new QAction(MainWindow);
        actionPrev_page->setObjectName("actionPrev_page");
        QIcon icon2(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious));
        actionPrev_page->setIcon(icon2);
        actionNext_page = new QAction(MainWindow);
        actionNext_page->setObjectName("actionNext_page");
        QIcon icon3(QIcon::fromTheme(QIcon::ThemeIcon::GoNext));
        actionNext_page->setIcon(icon3);
        actionZoom_In = new QAction(MainWindow);
        actionZoom_In->setObjectName("actionZoom_In");
        QIcon icon4(QIcon::fromTheme(QIcon::ThemeIcon::ZoomIn));
        actionZoom_In->setIcon(icon4);
        actionZoom_Out = new QAction(MainWindow);
        actionZoom_Out->setObjectName("actionZoom_Out");
        QIcon icon5(QIcon::fromTheme(QIcon::ThemeIcon::ZoomOut));
        actionZoom_Out->setIcon(icon5);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        actionHand_Tool = new QAction(MainWindow);
        actionHand_Tool->setObjectName("actionHand_Tool");
        actionHand_Tool->setCheckable(true);
        actionHand_Tool->setChecked(true);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/resources/hand.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionHand_Tool->setIcon(icon6);
        actionContents = new QAction(MainWindow);
        actionContents->setObjectName("actionContents");
        actionContents->setCheckable(true);
        actionContents->setChecked(true);
        actionBookmars = new QAction(MainWindow);
        actionBookmars->setObjectName("actionBookmars");
        actionBookmars->setCheckable(true);
        actionBookmars->setChecked(true);
        centralwidget = new QTabWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setTabsClosable(true);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 24));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName("menu_File");
        menuNavigation = new QMenu(menubar);
        menuNavigation->setObjectName("menuNavigation");
        menuView = new QMenu(menubar);
        menuView->setObjectName("menuView");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        menuTools = new QMenu(menubar);
        menuTools->setObjectName("menuTools");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName("toolBar");
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);
        contents_dockWidget = new QDockWidget(MainWindow);
        contents_dockWidget->setObjectName("contents_dockWidget");
        contents_treeView = new ContentsTreeView();
        contents_treeView->setObjectName("contents_treeView");
        verticalLayout = new QVBoxLayout(contents_treeView);
        verticalLayout->setObjectName("verticalLayout");
        contents_dockWidget->setWidget(contents_treeView);
        MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, contents_dockWidget);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menuNavigation->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuTools->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menu_File->addAction(action_Open);
        menuNavigation->addAction(actionPrev_page);
        menuNavigation->addAction(actionNext_page);
        menuNavigation->addAction(actionZoom_In);
        menuNavigation->addAction(actionZoom_Out);
        menuView->addAction(actionContents);
        menuView->addAction(actionBookmars);
        menuHelp->addAction(actionAbout);
        menuTools->addAction(actionHand_Tool);
        toolBar->addAction(action_Open);
        toolBar->addSeparator();
        toolBar->addAction(actionPrev_page);
        toolBar->addAction(actionNext_page);
        toolBar->addAction(actionZoom_In);
        toolBar->addAction(actionZoom_Out);
        toolBar->addSeparator();
        toolBar->addAction(actionHand_Tool);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "SPReader", nullptr));
        action_Open->setText(QCoreApplication::translate("MainWindow", "&Open", nullptr));
        actionPrev_page->setText(QCoreApplication::translate("MainWindow", "Prev page", nullptr));
        actionNext_page->setText(QCoreApplication::translate("MainWindow", "Next page", nullptr));
        actionZoom_In->setText(QCoreApplication::translate("MainWindow", "Zoom In", nullptr));
        actionZoom_Out->setText(QCoreApplication::translate("MainWindow", "Zoom Out", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        actionHand_Tool->setText(QCoreApplication::translate("MainWindow", "Hand Tool", nullptr));
        actionContents->setText(QCoreApplication::translate("MainWindow", "Contents", nullptr));
        actionBookmars->setText(QCoreApplication::translate("MainWindow", "Bookmars", nullptr));
        menu_File->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
        menuNavigation->setTitle(QCoreApplication::translate("MainWindow", "Navigaion", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
        menuTools->setTitle(QCoreApplication::translate("MainWindow", "Tools", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
        contents_dockWidget->setWindowTitle(QCoreApplication::translate("MainWindow", "Contents", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
