/********************************************************************************
** Form generated from reading UI file 'setmenu.ui'
**
** Created: Wed May 10 17:29:39 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETMENU_H
#define UI_SETMENU_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SetMenu
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *adduser;

    void setupUi(QWidget *SetMenu)
    {
        if (SetMenu->objectName().isEmpty())
            SetMenu->setObjectName(QString::fromUtf8("SetMenu"));
        SetMenu->setEnabled(true);
        SetMenu->resize(400, 300);
        pushButton = new QPushButton(SetMenu);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(50, 30, 99, 27));
        pushButton_2 = new QPushButton(SetMenu);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(50, 120, 99, 27));
        adduser = new QPushButton(SetMenu);
        adduser->setObjectName(QString::fromUtf8("adduser"));
        adduser->setGeometry(QRect(50, 200, 99, 27));

        retranslateUi(SetMenu);

        QMetaObject::connectSlotsByName(SetMenu);
    } // setupUi

    void retranslateUi(QWidget *SetMenu)
    {
        SetMenu->setWindowTitle(QApplication::translate("SetMenu", "Form", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("SetMenu", "\345\255\230\345\205\245", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("SetMenu", "\345\217\226\345\207\272", 0, QApplication::UnicodeUTF8));
        adduser->setText(QApplication::translate("SetMenu", "\346\267\273\345\212\240\347\224\250\346\210\267", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SetMenu: public Ui_SetMenu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETMENU_H
