/********************************************************************************
** Form generated from reading UI file 'MXExp.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MXEXP_H
#define UI_MXEXP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DockWidget
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame;
    QToolButton *toolButton;
    QToolButton *toolButton_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QTreeView *treeView;

    void setupUi(QDockWidget *DockWidget)
    {
        if (DockWidget->objectName().isEmpty())
            DockWidget->setObjectName(QStringLiteral("DockWidget"));
        DockWidget->resize(400, 358);
        DockWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout_2 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_2->setSpacing(3);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
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
        icon.addFile(QStringLiteral(":/CMain/Resources/60.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton->setIcon(icon);
        toolButton->setAutoRaise(true);
        toolButton_2 = new QToolButton(frame);
        toolButton_2->setObjectName(QStringLiteral("toolButton_2"));
        toolButton_2->setGeometry(QRect(24, 0, 21, 18));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/CMain/Resources/59.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_2->setIcon(icon1);
        toolButton_2->setAutoRaise(true);

        verticalLayout_2->addWidget(frame);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(dockWidgetContents);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout->addWidget(lineEdit);


        verticalLayout_2->addLayout(horizontalLayout);

        lineEdit_2 = new QLineEdit(dockWidgetContents);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setAcceptDrops(false);
        lineEdit_2->setReadOnly(true);

        verticalLayout_2->addWidget(lineEdit_2);

        treeView = new QTreeView(dockWidgetContents);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeView->setAlternatingRowColors(true);
        treeView->setSelectionMode(QAbstractItemView::MultiSelection);
        treeView->setRootIsDecorated(false);
        treeView->setItemsExpandable(false);
        treeView->setExpandsOnDoubleClick(false);

        verticalLayout_2->addWidget(treeView);

        DockWidget->setWidget(dockWidgetContents);

        retranslateUi(DockWidget);

        QMetaObject::connectSlotsByName(DockWidget);
    } // setupUi

    void retranslateUi(QDockWidget *DockWidget)
    {
        DockWidget->setWindowTitle(QApplication::translate("DockWidget", "Expression Evaluator", 0));
        label->setText(QApplication::translate("DockWidget", "Expression:", 0));
    } // retranslateUi

};

namespace Ui {
    class DockWidget: public Ui_DockWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MXEXP_H
