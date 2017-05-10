/********************************************************************************
** Form generated from reading UI file 'cabinet.ui'
**
** Created: Wed May 10 17:29:39 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CABINET_H
#define UI_CABINET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Cabinet
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *logo;
    QTableWidget *tableWidget;

    void setupUi(QWidget *Cabinet)
    {
        if (Cabinet->objectName().isEmpty())
            Cabinet->setObjectName(QString::fromUtf8("Cabinet"));
        Cabinet->resize(200, 600);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Cabinet->sizePolicy().hasHeightForWidth());
        Cabinet->setSizePolicy(sizePolicy);
        Cabinet->setMinimumSize(QSize(200, 600));
        Cabinet->setMaximumSize(QSize(200, 16777215));
        verticalLayout = new QVBoxLayout(Cabinet);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        logo = new QLabel(Cabinet);
        logo->setObjectName(QString::fromUtf8("logo"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(logo->sizePolicy().hasHeightForWidth());
        logo->setSizePolicy(sizePolicy1);
        logo->setMinimumSize(QSize(0, 100));
        QFont font;
        font.setPointSize(19);
        logo->setFont(font);
        logo->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(logo);

        tableWidget = new QTableWidget(Cabinet);
        if (tableWidget->columnCount() < 1)
            tableWidget->setColumnCount(1);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setFrameShape(QFrame::StyledPanel);
        tableWidget->setAutoScroll(false);
        tableWidget->setColumnCount(1);
        tableWidget->horizontalHeader()->setVisible(false);
        tableWidget->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(tableWidget);


        retranslateUi(Cabinet);

        QMetaObject::connectSlotsByName(Cabinet);
    } // setupUi

    void retranslateUi(QWidget *Cabinet)
    {
        Cabinet->setWindowTitle(QApplication::translate("Cabinet", "Form", 0, QApplication::UnicodeUTF8));
        logo->setText(QApplication::translate("Cabinet", "\345\276\267\350\215\243\n"
"SPD", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Cabinet: public Ui_Cabinet {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CABINET_H
