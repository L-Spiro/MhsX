/********************************************************************************
** Form generated from reading UI file 'MXFa.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MXFA_H
#define UI_MXFA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FaDock
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QToolButton *toolButton;
    QToolButton *toolButton_2;
    QToolButton *toolButton_3;
    QTreeView *treeView;

    void setupUi(QDockWidget *FaDock)
    {
        if (FaDock->objectName().isEmpty())
            FaDock->setObjectName(QStringLiteral("FaDock"));
        FaDock->resize(400, 300);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(dockWidgetContents);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setMinimumSize(QSize(0, 18));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Plain);
        frame->setLineWidth(0);
        toolButton = new QToolButton(frame);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        toolButton->setGeometry(QRect(2, 0, 21, 18));
        QIcon icon;
        icon.addFile(QStringLiteral(":/CMain/Resources/65.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton->setIcon(icon);
        toolButton->setAutoRaise(true);
        toolButton_2 = new QToolButton(frame);
        toolButton_2->setObjectName(QStringLiteral("toolButton_2"));
        toolButton_2->setGeometry(QRect(24, 0, 21, 18));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/CMain/Resources/98.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_2->setIcon(icon1);
        toolButton_2->setAutoRaise(true);
        toolButton_3 = new QToolButton(frame);
        toolButton_3->setObjectName(QStringLiteral("toolButton_3"));
        toolButton_3->setGeometry(QRect(60, 0, 21, 18));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/CMain/Resources/46.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_3->setIcon(icon2);
        toolButton_3->setAutoRaise(true);

        verticalLayout->addWidget(frame);

        treeView = new QTreeView(dockWidgetContents);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView->setAlternatingRowColors(true);
        treeView->setSelectionMode(QAbstractItemView::MultiSelection);
        treeView->setRootIsDecorated(false);
        treeView->setItemsExpandable(false);
        treeView->setExpandsOnDoubleClick(false);

        verticalLayout->addWidget(treeView);

        FaDock->setWidget(dockWidgetContents);

        retranslateUi(FaDock);

        QMetaObject::connectSlotsByName(FaDock);
    } // setupUi

    void retranslateUi(QDockWidget *FaDock)
    {
        FaDock->setWindowTitle(QApplication::translate("FaDock", "Found Addresses", 0));
        toolButton_3->setText(QApplication::translate("FaDock", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class FaDock: public Ui_FaDock {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MXFA_H
