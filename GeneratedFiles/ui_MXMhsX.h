/********************************************************************************
** Form generated from reading UI file 'MXMhsX.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MXMHSX_H
#define UI_MXMHSX_H

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
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CMainClass
{
public:
    QAction *actionOpen_Process;
    QAction *actionOpen_Recent;
    QAction *actionOpen_for_Debug;
    QAction *actionAdd_an_Entry;
    QAction *action_Open_Save_File;
    QAction *action_Save;
    QAction *actionSave_As;
    QAction *actionRemove_Selected;
    QAction *actionRemove_All;
    QAction *actionLock_Selected;
    QAction *actionUnlock_Selected;
    QAction *action_Expression_Evaluator;
    QAction *actionShow_All;
    QAction *action_Found_Addresses;
    QAction *action_Data_Type_Search;
    QAction *action_Pointer_Search;
    QAction *action_String_Search;
    QAction *action_Group_Search;
    QAction *action_Expression_Search;
    QAction *actionS_ub_Search;
    QAction *action_Insert_Address;
    QAction *actionSearch_Options;
    QAction *action_PE_Works;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTreeView *treeView;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Windows;
    QMenu *menu_Search;
    QMenu *menu_Tools;
    QMenu *menu_Help;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *CMainClass)
    {
        if (CMainClass->objectName().isEmpty())
            CMainClass->setObjectName(QStringLiteral("CMainClass"));
        CMainClass->resize(800, 600);
        actionOpen_Process = new QAction(CMainClass);
        actionOpen_Process->setObjectName(QStringLiteral("actionOpen_Process"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/CMain/Resources/73.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_Process->setIcon(icon);
        actionOpen_Recent = new QAction(CMainClass);
        actionOpen_Recent->setObjectName(QStringLiteral("actionOpen_Recent"));
        actionOpen_for_Debug = new QAction(CMainClass);
        actionOpen_for_Debug->setObjectName(QStringLiteral("actionOpen_for_Debug"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/CMain/Resources/97.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_for_Debug->setIcon(icon1);
        actionAdd_an_Entry = new QAction(CMainClass);
        actionAdd_an_Entry->setObjectName(QStringLiteral("actionAdd_an_Entry"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/CMain/Resources/1.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAdd_an_Entry->setIcon(icon2);
        action_Open_Save_File = new QAction(CMainClass);
        action_Open_Save_File->setObjectName(QStringLiteral("action_Open_Save_File"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/CMain/Resources/52.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Open_Save_File->setIcon(icon3);
        action_Save = new QAction(CMainClass);
        action_Save->setObjectName(QStringLiteral("action_Save"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/CMain/Resources/22.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Save->setIcon(icon4);
        actionSave_As = new QAction(CMainClass);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/CMain/Resources/86.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_As->setIcon(icon5);
        actionRemove_Selected = new QAction(CMainClass);
        actionRemove_Selected->setObjectName(QStringLiteral("actionRemove_Selected"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/CMain/Resources/33.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRemove_Selected->setIcon(icon6);
        actionRemove_All = new QAction(CMainClass);
        actionRemove_All->setObjectName(QStringLiteral("actionRemove_All"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/CMain/Resources/31.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRemove_All->setIcon(icon7);
        actionLock_Selected = new QAction(CMainClass);
        actionLock_Selected->setObjectName(QStringLiteral("actionLock_Selected"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/CMain/Resources/27.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLock_Selected->setIcon(icon8);
        actionUnlock_Selected = new QAction(CMainClass);
        actionUnlock_Selected->setObjectName(QStringLiteral("actionUnlock_Selected"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/CMain/Resources/28.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUnlock_Selected->setIcon(icon9);
        action_Expression_Evaluator = new QAction(CMainClass);
        action_Expression_Evaluator->setObjectName(QStringLiteral("action_Expression_Evaluator"));
        action_Expression_Evaluator->setCheckable(true);
        actionShow_All = new QAction(CMainClass);
        actionShow_All->setObjectName(QStringLiteral("actionShow_All"));
        action_Found_Addresses = new QAction(CMainClass);
        action_Found_Addresses->setObjectName(QStringLiteral("action_Found_Addresses"));
        action_Found_Addresses->setCheckable(true);
        action_Data_Type_Search = new QAction(CMainClass);
        action_Data_Type_Search->setObjectName(QStringLiteral("action_Data_Type_Search"));
        action_Pointer_Search = new QAction(CMainClass);
        action_Pointer_Search->setObjectName(QStringLiteral("action_Pointer_Search"));
        action_String_Search = new QAction(CMainClass);
        action_String_Search->setObjectName(QStringLiteral("action_String_Search"));
        action_Group_Search = new QAction(CMainClass);
        action_Group_Search->setObjectName(QStringLiteral("action_Group_Search"));
        action_Expression_Search = new QAction(CMainClass);
        action_Expression_Search->setObjectName(QStringLiteral("action_Expression_Search"));
        actionS_ub_Search = new QAction(CMainClass);
        actionS_ub_Search->setObjectName(QStringLiteral("actionS_ub_Search"));
        action_Insert_Address = new QAction(CMainClass);
        action_Insert_Address->setObjectName(QStringLiteral("action_Insert_Address"));
        actionSearch_Options = new QAction(CMainClass);
        actionSearch_Options->setObjectName(QStringLiteral("actionSearch_Options"));
        action_PE_Works = new QAction(CMainClass);
        action_PE_Works->setObjectName(QStringLiteral("action_PE_Works"));
        centralWidget = new QWidget(CMainClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(3);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        treeView = new QTreeView(centralWidget);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView->setAlternatingRowColors(true);
        treeView->setSelectionMode(QAbstractItemView::MultiSelection);
        treeView->setRootIsDecorated(false);
        treeView->setItemsExpandable(false);
        treeView->setExpandsOnDoubleClick(false);
        treeView->header()->setStretchLastSection(true);

        horizontalLayout->addWidget(treeView);

        CMainClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(CMainClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 21));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_Windows = new QMenu(menuBar);
        menu_Windows->setObjectName(QStringLiteral("menu_Windows"));
        menu_Search = new QMenu(menuBar);
        menu_Search->setObjectName(QStringLiteral("menu_Search"));
        menu_Tools = new QMenu(menuBar);
        menu_Tools->setObjectName(QStringLiteral("menu_Tools"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QStringLiteral("menu_Help"));
        CMainClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CMainClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setFloatable(true);
        CMainClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(CMainClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        CMainClass->setStatusBar(statusBar);
        toolBar = new QToolBar(CMainClass);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        CMainClass->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Search->menuAction());
        menuBar->addAction(menu_Tools->menuAction());
        menuBar->addAction(menu_Windows->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_File->addAction(actionOpen_Process);
        menu_File->addAction(actionOpen_Recent);
        menu_File->addAction(actionOpen_for_Debug);
        menu_File->addSeparator();
        menu_File->addAction(action_Open_Save_File);
        menu_File->addAction(action_Save);
        menu_File->addAction(actionSave_As);
        menu_Windows->addAction(actionShow_All);
        menu_Windows->addSeparator();
        menu_Windows->addAction(action_Expression_Evaluator);
        menu_Windows->addAction(action_Found_Addresses);
        menu_Search->addAction(action_Data_Type_Search);
        menu_Search->addAction(action_Pointer_Search);
        menu_Search->addAction(action_String_Search);
        menu_Search->addAction(action_Group_Search);
        menu_Search->addAction(action_Expression_Search);
        menu_Search->addSeparator();
        menu_Search->addAction(actionS_ub_Search);
        menu_Search->addSeparator();
        menu_Search->addAction(action_Insert_Address);
        menu_Search->addSeparator();
        menu_Search->addAction(actionSearch_Options);
        menu_Tools->addAction(action_PE_Works);
        mainToolBar->addAction(actionOpen_Process);
        mainToolBar->addAction(actionOpen_for_Debug);
        toolBar->addAction(actionAdd_an_Entry);
        toolBar->addAction(action_Open_Save_File);
        toolBar->addAction(action_Save);
        toolBar->addSeparator();
        toolBar->addAction(actionRemove_Selected);
        toolBar->addAction(actionRemove_All);
        toolBar->addSeparator();
        toolBar->addAction(actionLock_Selected);
        toolBar->addAction(actionUnlock_Selected);

        retranslateUi(CMainClass);

        QMetaObject::connectSlotsByName(CMainClass);
    } // setupUi

    void retranslateUi(QMainWindow *CMainClass)
    {
        CMainClass->setWindowTitle(QApplication::translate("CMainClass", "\302\240", 0));
        actionOpen_Process->setText(QApplication::translate("CMainClass", "Open &Process", 0));
#ifndef QT_NO_TOOLTIP
        actionOpen_Process->setToolTip(QApplication::translate("CMainClass", "Open a process.", 0));
#endif // QT_NO_TOOLTIP
        actionOpen_Process->setShortcut(QApplication::translate("CMainClass", "Ctrl+P", 0));
        actionOpen_Recent->setText(QApplication::translate("CMainClass", "Open &Recent", 0));
#ifndef QT_NO_TOOLTIP
        actionOpen_Recent->setToolTip(QApplication::translate("CMainClass", "Open a recent process.", 0));
#endif // QT_NO_TOOLTIP
        actionOpen_for_Debug->setText(QApplication::translate("CMainClass", "Open for &Debug", 0));
#ifndef QT_NO_TOOLTIP
        actionOpen_for_Debug->setToolTip(QApplication::translate("CMainClass", "Open a process for debug.", 0));
#endif // QT_NO_TOOLTIP
        actionAdd_an_Entry->setText(QApplication::translate("CMainClass", "Add an Entry", 0));
#ifndef QT_NO_TOOLTIP
        actionAdd_an_Entry->setToolTip(QApplication::translate("CMainClass", "Add an entry.", 0));
#endif // QT_NO_TOOLTIP
        action_Open_Save_File->setText(QApplication::translate("CMainClass", "&Open Save File", 0));
#ifndef QT_NO_TOOLTIP
        action_Open_Save_File->setToolTip(QApplication::translate("CMainClass", "Open a save file.", 0));
#endif // QT_NO_TOOLTIP
        action_Open_Save_File->setShortcut(QApplication::translate("CMainClass", "Ctrl+O", 0));
        action_Save->setText(QApplication::translate("CMainClass", "&Save", 0));
#ifndef QT_NO_TOOLTIP
        action_Save->setToolTip(QApplication::translate("CMainClass", "Save results.", 0));
#endif // QT_NO_TOOLTIP
        action_Save->setShortcut(QApplication::translate("CMainClass", "Ctrl+S", 0));
        actionSave_As->setText(QApplication::translate("CMainClass", "Save &As", 0));
#ifndef QT_NO_TOOLTIP
        actionSave_As->setToolTip(QApplication::translate("CMainClass", "Save entries as a given file name.", 0));
#endif // QT_NO_TOOLTIP
        actionSave_As->setShortcut(QApplication::translate("CMainClass", "Ctrl+A", 0));
        actionRemove_Selected->setText(QApplication::translate("CMainClass", "Remove Selected", 0));
#ifndef QT_NO_TOOLTIP
        actionRemove_Selected->setToolTip(QApplication::translate("CMainClass", "Remove the selected entries.", 0));
#endif // QT_NO_TOOLTIP
        actionRemove_All->setText(QApplication::translate("CMainClass", "Remove All", 0));
#ifndef QT_NO_TOOLTIP
        actionRemove_All->setToolTip(QApplication::translate("CMainClass", "Remove all entries.", 0));
#endif // QT_NO_TOOLTIP
        actionLock_Selected->setText(QApplication::translate("CMainClass", "Lock Selected", 0));
#ifndef QT_NO_TOOLTIP
        actionLock_Selected->setToolTip(QApplication::translate("CMainClass", "Lock the selected entries.", 0));
#endif // QT_NO_TOOLTIP
        actionUnlock_Selected->setText(QApplication::translate("CMainClass", "Unlock Selected", 0));
#ifndef QT_NO_TOOLTIP
        actionUnlock_Selected->setToolTip(QApplication::translate("CMainClass", "Unlock the selected entries.", 0));
#endif // QT_NO_TOOLTIP
        action_Expression_Evaluator->setText(QApplication::translate("CMainClass", "&Expression Evaluator", 0));
#ifndef QT_NO_TOOLTIP
        action_Expression_Evaluator->setToolTip(QApplication::translate("CMainClass", "Show the Expression Evaluator.", 0));
#endif // QT_NO_TOOLTIP
        actionShow_All->setText(QApplication::translate("CMainClass", "&Show All", 0));
#ifndef QT_NO_TOOLTIP
        actionShow_All->setToolTip(QApplication::translate("CMainClass", "Show all windows.", 0));
#endif // QT_NO_TOOLTIP
        action_Found_Addresses->setText(QApplication::translate("CMainClass", "&Found Addresses", 0));
        action_Data_Type_Search->setText(QApplication::translate("CMainClass", "&Data-Type Search", 0));
        action_Data_Type_Search->setShortcut(QApplication::translate("CMainClass", "Ctrl+Shift+S", 0));
        action_Pointer_Search->setText(QApplication::translate("CMainClass", "&Pointer Search", 0));
        action_Pointer_Search->setShortcut(QApplication::translate("CMainClass", "Ctrl+Shift+S", 0));
        action_String_Search->setText(QApplication::translate("CMainClass", "&String Search", 0));
        action_Group_Search->setText(QApplication::translate("CMainClass", "&Group Search", 0));
        action_Expression_Search->setText(QApplication::translate("CMainClass", "&Expression Search", 0));
        actionS_ub_Search->setText(QApplication::translate("CMainClass", "S&ub Search", 0));
        actionS_ub_Search->setShortcut(QApplication::translate("CMainClass", "Ctrl+Shift+D", 0));
        action_Insert_Address->setText(QApplication::translate("CMainClass", "&Insert Address", 0));
        action_Insert_Address->setShortcut(QApplication::translate("CMainClass", "Ins", 0));
        actionSearch_Options->setText(QApplication::translate("CMainClass", "Search &Options", 0));
        action_PE_Works->setText(QApplication::translate("CMainClass", "&PE Works", 0));
#ifndef QT_NO_TOOLTIP
        action_PE_Works->setToolTip(QApplication::translate("CMainClass", "PE Works.", 0));
#endif // QT_NO_TOOLTIP
        menu_File->setTitle(QApplication::translate("CMainClass", "&File", 0));
        menu_Windows->setTitle(QApplication::translate("CMainClass", "&Window", 0));
        menu_Search->setTitle(QApplication::translate("CMainClass", "&Search", 0));
        menu_Tools->setTitle(QApplication::translate("CMainClass", "&Tools", 0));
        menu_Help->setTitle(QApplication::translate("CMainClass", "&Help", 0));
        mainToolBar->setWindowTitle(QApplication::translate("CMainClass", "Menu Bar", 0));
        toolBar->setWindowTitle(QApplication::translate("CMainClass", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class CMainClass: public Ui_CMainClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MXMHSX_H
