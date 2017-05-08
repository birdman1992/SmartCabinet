/********************************************************************************
** Form generated from reading UI file 'primaryuser.ui'
**
** Created: Mon May 8 11:00:55 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRIMARYUSER_H
#define UI_PRIMARYUSER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PrimaryUser
{
public:
    QPushButton *sure;
    QPushButton *quit;
    QLabel *label;
    QLabel *name;
    QLabel *label_3;
    QLabel *authority;

    void setupUi(QWidget *PrimaryUser)
    {
        if (PrimaryUser->objectName().isEmpty())
            PrimaryUser->setObjectName(QString::fromUtf8("PrimaryUser"));
        PrimaryUser->resize(400, 300);
        sure = new QPushButton(PrimaryUser);
        sure->setObjectName(QString::fromUtf8("sure"));
        sure->setGeometry(QRect(70, 230, 99, 27));
        quit = new QPushButton(PrimaryUser);
        quit->setObjectName(QString::fromUtf8("quit"));
        quit->setGeometry(QRect(200, 230, 99, 27));
        label = new QLabel(PrimaryUser);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(50, 20, 67, 17));
        name = new QLabel(PrimaryUser);
        name->setObjectName(QString::fromUtf8("name"));
        name->setGeometry(QRect(160, 20, 67, 17));
        label_3 = new QLabel(PrimaryUser);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(50, 60, 67, 17));
        authority = new QLabel(PrimaryUser);
        authority->setObjectName(QString::fromUtf8("authority"));
        authority->setGeometry(QRect(160, 60, 67, 17));

        retranslateUi(PrimaryUser);

        QMetaObject::connectSlotsByName(PrimaryUser);
    } // setupUi

    void retranslateUi(QWidget *PrimaryUser)
    {
        PrimaryUser->setWindowTitle(QApplication::translate("PrimaryUser", "Form", 0, QApplication::UnicodeUTF8));
        sure->setText(QApplication::translate("PrimaryUser", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
        quit->setText(QApplication::translate("PrimaryUser", "\351\200\200\345\207\272", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PrimaryUser", "\345\247\223\345\220\215", 0, QApplication::UnicodeUTF8));
        name->setText(QApplication::translate("PrimaryUser", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PrimaryUser", "\350\201\214\344\275\215", 0, QApplication::UnicodeUTF8));
        authority->setText(QApplication::translate("PrimaryUser", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PrimaryUser: public Ui_PrimaryUser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRIMARYUSER_H
