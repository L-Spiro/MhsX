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
    QHBoxLayout *hla2;
    QHBoxLayout *hla;
    QTreeView *tv;

    void setupUi(QWidget *PeW)
    {
        if (PeW->objectName().isEmpty())
            PeW->setObjectName(QStringLiteral("PeW"));
        PeW->resize(400, 300);
        hla2 = new QHBoxLayout(PeW);
        hla2->setSpacing(3);
        hla2->setObjectName(QStringLiteral("hla2"));
        hla2->setContentsMargins(0, 0, 0, 0);
        hla = new QHBoxLayout();
        hla->setSpacing(3);
        hla->setObjectName(QStringLiteral("hla"));
        tv = new QTreeView(PeW);
        tv->setObjectName(QStringLiteral("tv"));

        hla->addWidget(tv);


        hla2->addLayout(hla);


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
