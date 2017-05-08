/********************************************************************************
** Form generated from reading UI file 'standbywidget.ui'
**
** Created: Mon May 8 11:00:55 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STANDBYWIDGET_H
#define UI_STANDBYWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StandbyWidget
{
public:
    QPushButton *pushButton;

    void setupUi(QWidget *StandbyWidget)
    {
        if (StandbyWidget->objectName().isEmpty())
            StandbyWidget->setObjectName(QString::fromUtf8("StandbyWidget"));
        StandbyWidget->resize(833, 557);
        pushButton = new QPushButton(StandbyWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(130, 140, 141, 101));

        retranslateUi(StandbyWidget);

        QMetaObject::connectSlotsByName(StandbyWidget);
    } // setupUi

    void retranslateUi(QWidget *StandbyWidget)
    {
        StandbyWidget->setWindowTitle(QApplication::translate("StandbyWidget", "Form", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("StandbyWidget", "standby", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class StandbyWidget: public Ui_StandbyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STANDBYWIDGET_H
