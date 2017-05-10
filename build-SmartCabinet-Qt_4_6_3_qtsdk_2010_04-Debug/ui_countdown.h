/********************************************************************************
** Form generated from reading UI file 'countdown.ui'
**
** Created: Wed May 10 17:29:39 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COUNTDOWN_H
#define UI_COUNTDOWN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CountDown
{
public:
    QLabel *label;
    QLabel *label_2;

    void setupUi(QWidget *CountDown)
    {
        if (CountDown->objectName().isEmpty())
            CountDown->setObjectName(QString::fromUtf8("CountDown"));
        CountDown->resize(400, 300);
        label = new QLabel(CountDown);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 69, 401, 31));
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(CountDown);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 150, 401, 20));
        label_2->setAlignment(Qt::AlignCenter);

        retranslateUi(CountDown);

        QMetaObject::connectSlotsByName(CountDown);
    } // setupUi

    void retranslateUi(QWidget *CountDown)
    {
        CountDown->setWindowTitle(QApplication::translate("CountDown", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("CountDown", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CountDown: public Ui_CountDown {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COUNTDOWN_H
