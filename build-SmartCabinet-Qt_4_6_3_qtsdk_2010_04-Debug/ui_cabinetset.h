/********************************************************************************
** Form generated from reading UI file 'cabinetset.ui'
**
** Created: Wed May 10 17:29:39 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CABINETSET_H
#define UI_CABINETSET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CabinetSet
{
public:
    QLabel *label_0;
    QLabel *label_2;
    QLabel *label_4;
    QLabel *label_1;
    QLabel *label_3;
    QPushButton *add_left;
    QPushButton *add_right;
    QPushButton *clear;
    QPushButton *save;
    QPushButton *cancel;

    void setupUi(QWidget *CabinetSet)
    {
        if (CabinetSet->objectName().isEmpty())
            CabinetSet->setObjectName(QString::fromUtf8("CabinetSet"));
        CabinetSet->resize(1024, 600);
        label_0 = new QLabel(CabinetSet);
        label_0->setObjectName(QString::fromUtf8("label_0"));
        label_0->setGeometry(QRect(460, 160, 91, 391));
        QFont font;
        font.setPointSize(40);
        label_0->setFont(font);
        label_0->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 170, 127);"));
        label_0->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(CabinetSet);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(560, 160, 91, 391));
        label_2->setFont(font);
        label_2->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 170, 127);"));
        label_2->setAlignment(Qt::AlignCenter);
        label_4 = new QLabel(CabinetSet);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(660, 160, 91, 391));
        label_4->setFont(font);
        label_4->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 170, 127);"));
        label_4->setAlignment(Qt::AlignCenter);
        label_1 = new QLabel(CabinetSet);
        label_1->setObjectName(QString::fromUtf8("label_1"));
        label_1->setGeometry(QRect(360, 160, 91, 391));
        label_1->setFont(font);
        label_1->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 170, 127);"));
        label_1->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(CabinetSet);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(260, 160, 91, 391));
        label_3->setFont(font);
        label_3->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 170, 127);"));
        label_3->setAlignment(Qt::AlignCenter);
        add_left = new QPushButton(CabinetSet);
        add_left->setObjectName(QString::fromUtf8("add_left"));
        add_left->setGeometry(QRect(260, 80, 91, 61));
        QFont font1;
        font1.setPointSize(20);
        add_left->setFont(font1);
        add_right = new QPushButton(CabinetSet);
        add_right->setObjectName(QString::fromUtf8("add_right"));
        add_right->setGeometry(QRect(660, 80, 91, 61));
        add_right->setFont(font1);
        clear = new QPushButton(CabinetSet);
        clear->setObjectName(QString::fromUtf8("clear"));
        clear->setGeometry(QRect(460, 80, 91, 61));
        clear->setFont(font1);
        save = new QPushButton(CabinetSet);
        save->setObjectName(QString::fromUtf8("save"));
        save->setGeometry(QRect(880, 80, 91, 61));
        save->setFont(font1);
        cancel = new QPushButton(CabinetSet);
        cancel->setObjectName(QString::fromUtf8("cancel"));
        cancel->setGeometry(QRect(40, 80, 91, 61));
        cancel->setFont(font1);

        retranslateUi(CabinetSet);

        QMetaObject::connectSlotsByName(CabinetSet);
    } // setupUi

    void retranslateUi(QWidget *CabinetSet)
    {
        CabinetSet->setWindowTitle(QApplication::translate("CabinetSet", "Form", 0, QApplication::UnicodeUTF8));
        label_0->setText(QApplication::translate("CabinetSet", "\344\270\273\n"
"\346\237\234", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("CabinetSet", "\345\211\257\n"
"\346\237\234", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("CabinetSet", "\345\211\257\n"
"\346\237\234", 0, QApplication::UnicodeUTF8));
        label_1->setText(QApplication::translate("CabinetSet", "\345\211\257\n"
"\346\237\234", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("CabinetSet", "\345\211\257\n"
"\346\237\234", 0, QApplication::UnicodeUTF8));
        add_left->setText(QApplication::translate("CabinetSet", "<--+", 0, QApplication::UnicodeUTF8));
        add_right->setText(QApplication::translate("CabinetSet", "+-->", 0, QApplication::UnicodeUTF8));
        clear->setText(QApplication::translate("CabinetSet", "-", 0, QApplication::UnicodeUTF8));
        save->setText(QApplication::translate("CabinetSet", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        cancel->setText(QApplication::translate("CabinetSet", "\345\217\226\346\266\210", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CabinetSet: public Ui_CabinetSet {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CABINETSET_H
