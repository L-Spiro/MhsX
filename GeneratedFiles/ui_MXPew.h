/********************************************************************************
** Form generated from reading UI file 'MXPew.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MXPEW_H
#define UI_MXPEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PeWin
{
public:
    QAction *action_Open;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabWidget;
    QMenuBar *menubar;
    QMenu *menu_File;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *PeWin)
    {
        if (PeWin->objectName().isEmpty())
            PeWin->setObjectName(QStringLiteral("PeWin"));
        PeWin->resize(800, 600);
        action_Open = new QAction(PeWin);
        action_Open->setObjectName(QStringLiteral("action_Open"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/CMain/Resources/52.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Open->setIcon(icon);
        centralwidget = new QWidget(PeWin);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout_2 = new QHBoxLayout(centralwidget);
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(3);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideRight);
        tabWidget->setDocumentMode(true);
        tabWidget->setTabsClosable(true);
        tabWidget->setMovable(false);
        tabWidget->setTabBarAutoHide(false);

        horizontalLayout->addWidget(tabWidget);


        horizontalLayout_2->addLayout(horizontalLayout);

        PeWin->setCentralWidget(centralwidget);
        menubar = new QMenuBar(PeWin);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        PeWin->setMenuBar(menubar);
        statusbar = new QStatusBar(PeWin);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        PeWin->setStatusBar(statusbar);
        toolBar = new QToolBar(PeWin);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        PeWin->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menu_File->menuAction());
        menu_File->addAction(action_Open);
        toolBar->addAction(action_Open);

        retranslateUi(PeWin);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(PeWin);
    } // setupUi

    void retranslateUi(QMainWindow *PeWin)
    {
        PeWin->setWindowTitle(QApplication::translate("PeWin", "PE\302\240Works", 0));
        action_Open->setText(QApplication::translate("PeWin", "&Open", 0));
        action_Open->setShortcut(QApplication::translate("PeWin", "Ctrl+O", 0));
        menu_File->setTitle(QApplication::translate("PeWin", "&File", 0));
        toolBar->setWindowTitle(QApplication::translate("PeWin", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class PeWin: public Ui_PeWin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MXPEW_H
