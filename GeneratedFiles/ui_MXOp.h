/********************************************************************************
** Form generated from reading UI file 'MXOp.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MXOP_H
#define UI_MXOP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OP
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QGroupBox *grB;
    QGridLayout *gridLayout_2;
    QGroupBox *gB2;
    QHBoxLayout *horizontalLayout;
    QRadioButton *rb0;
    QSpacerItem *horizontalSpacer;
    QRadioButton *radioButton;
    QTreeView *tv;

    void setupUi(QMainWindow *OP)
    {
        if (OP->objectName().isEmpty())
            OP->setObjectName(QStringLiteral("OP"));
        OP->setWindowModality(Qt::ApplicationModal);
        OP->resize(586, 375);
        centralwidget = new QWidget(OP);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        grB = new QGroupBox(centralwidget);
        grB->setObjectName(QStringLiteral("grB"));
        grB->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        grB->setFlat(false);
        grB->setCheckable(false);
        gridLayout_2 = new QGridLayout(grB);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gB2 = new QGroupBox(grB);
        gB2->setObjectName(QStringLiteral("gB2"));
        gB2->setMinimumSize(QSize(0, 40));
        horizontalLayout = new QHBoxLayout(gB2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        rb0 = new QRadioButton(gB2);
        rb0->setObjectName(QStringLiteral("rb0"));
        rb0->setChecked(true);

        horizontalLayout->addWidget(rb0);

        horizontalSpacer = new QSpacerItem(429, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        radioButton = new QRadioButton(gB2);
        radioButton->setObjectName(QStringLiteral("radioButton"));

        horizontalLayout->addWidget(radioButton);


        gridLayout_2->addWidget(gB2, 1, 0, 1, 1);

        tv = new QTreeView(grB);
        tv->setObjectName(QStringLiteral("tv"));
        tv->header()->setProperty("showSortIndicator", QVariant(true));

        gridLayout_2->addWidget(tv, 0, 0, 1, 1);


        gridLayout->addWidget(grB, 0, 0, 1, 1);

        OP->setCentralWidget(centralwidget);

        retranslateUi(OP);

        QMetaObject::connectSlotsByName(OP);
    } // setupUi

    void retranslateUi(QMainWindow *OP)
    {
        OP->setWindowTitle(QApplication::translate("OP", "Open Process", 0));
        grB->setTitle(QApplication::translate("OP", "Select Process", 0));
        gB2->setTitle(QApplication::translate("OP", "Show", 0));
        rb0->setText(QApplication::translate("OP", "Main", 0));
        radioButton->setText(QApplication::translate("OP", "All", 0));
    } // retranslateUi

};

namespace Ui {
    class OP: public Ui_OP {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MXOP_H
