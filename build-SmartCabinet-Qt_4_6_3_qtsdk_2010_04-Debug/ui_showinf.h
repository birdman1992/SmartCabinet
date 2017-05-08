/********************************************************************************
** Form generated from reading UI file 'showinf.ui'
**
** Created: Mon May 8 15:55:58 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWINF_H
#define UI_SHOWINF_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShowInf
{
public:
    QPushButton *save;
    QPushButton *close;
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *name;
    QLabel *label_3;
    QLabel *num;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *cabinet;
    QLabel *num_2;
    QLabel *exist;
    QPushButton *out;

    void setupUi(QWidget *ShowInf)
    {
        if (ShowInf->objectName().isEmpty())
            ShowInf->setObjectName(QString::fromUtf8("ShowInf"));
        ShowInf->resize(400, 300);
        save = new QPushButton(ShowInf);
        save->setObjectName(QString::fromUtf8("save"));
        save->setGeometry(QRect(30, 260, 99, 27));
        close = new QPushButton(ShowInf);
        close->setObjectName(QString::fromUtf8("close"));
        close->setGeometry(QRect(260, 260, 99, 27));
        groupBox = new QGroupBox(ShowInf);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 0, 381, 191));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 30, 67, 17));
        name = new QLabel(groupBox);
        name->setObjectName(QString::fromUtf8("name"));
        name->setGeometry(QRect(110, 30, 67, 17));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 70, 67, 17));
        num = new QLabel(groupBox);
        num->setObjectName(QString::fromUtf8("num"));
        num->setGeometry(QRect(110, 70, 67, 17));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 120, 67, 17));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 160, 67, 17));
        cabinet = new QLabel(groupBox);
        cabinet->setObjectName(QString::fromUtf8("cabinet"));
        cabinet->setGeometry(QRect(110, 120, 67, 17));
        num_2 = new QLabel(groupBox);
        num_2->setObjectName(QString::fromUtf8("num_2"));
        num_2->setGeometry(QRect(110, 160, 67, 17));
        label->raise();
        name->raise();
        label_3->raise();
        num->raise();
        label_5->raise();
        label_6->raise();
        cabinet->raise();
        num_2->raise();
        exist = new QLabel(ShowInf);
        exist->setObjectName(QString::fromUtf8("exist"));
        exist->setGeometry(QRect(0, 230, 401, 20));
        exist->setAlignment(Qt::AlignCenter);
        out = new QPushButton(ShowInf);
        out->setObjectName(QString::fromUtf8("out"));
        out->setGeometry(QRect(140, 260, 99, 27));

        retranslateUi(ShowInf);

        QMetaObject::connectSlotsByName(ShowInf);
    } // setupUi

    void retranslateUi(QWidget *ShowInf)
    {
        ShowInf->setWindowTitle(QApplication::translate("ShowInf", "Form", 0, QApplication::UnicodeUTF8));
        save->setText(QApplication::translate("ShowInf", "\345\255\230\345\205\245", 0, QApplication::UnicodeUTF8));
        close->setText(QApplication::translate("ShowInf", "\351\200\200\345\207\272", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ShowInf", "\350\215\257\345\223\201\344\273\213\347\273\215", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ShowInf", "\345\220\215\347\247\260", 0, QApplication::UnicodeUTF8));
        name->setText(QApplication::translate("ShowInf", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ShowInf", " \345\272\223\345\255\230", 0, QApplication::UnicodeUTF8));
        num->setText(QApplication::translate("ShowInf", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ShowInf", "\350\215\257\346\237\234", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ShowInf", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
        cabinet->setText(QApplication::translate("ShowInf", "TextLabel", 0, QApplication::UnicodeUTF8));
        num_2->setText(QApplication::translate("ShowInf", "TextLabel", 0, QApplication::UnicodeUTF8));
        exist->setText(QApplication::translate("ShowInf", "TextLabel", 0, QApplication::UnicodeUTF8));
        out->setText(QApplication::translate("ShowInf", "\345\217\226\345\207\272", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ShowInf: public Ui_ShowInf {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWINF_H
