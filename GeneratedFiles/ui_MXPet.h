/********************************************************************************
** Form generated from reading UI file 'MXPet.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MXPET_H
#define UI_MXPET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PeW
{
public:
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QTreeView *treeView;

    void setupUi(QWidget *PeW)
    {
        if (PeW->objectName().isEmpty())
            PeW->setObjectName(QStringLiteral("PeW"));
        PeW->resize(400, 300);
        horizontalLayout_2 = new QHBoxLayout(PeW);
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(3);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        treeView = new QTreeView(PeW);
        treeView->setObjectName(QStringLiteral("treeView"));

        horizontalLayout->addWidget(treeView);


        horizontalLayout_2->addLayout(horizontalLayout);


        retranslateUi(PeW);

        QMetaObject::connectSlotsByName(PeW);
    } // setupUi

    void retranslateUi(QWidget *PeW)
    {
        PeW->setWindowTitle(QApplication::translate("PeW", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class PeW: public Ui_PeW {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MXPET_H
