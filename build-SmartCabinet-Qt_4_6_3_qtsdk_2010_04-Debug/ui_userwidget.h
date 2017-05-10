/********************************************************************************
** Form generated from reading UI file 'userwidget.ui'
**
** Created: Wed May 10 17:29:39 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERWIDGET_H
#define UI_USERWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserWidget
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QPushButton *addUser;
    QPushButton *addOk;
    QTableWidget *tableWidget;

    void setupUi(QWidget *UserWidget)
    {
        if (UserWidget->objectName().isEmpty())
            UserWidget->setObjectName(QString::fromUtf8("UserWidget"));
        UserWidget->resize(1024, 600);
        verticalLayout = new QVBoxLayout(UserWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(UserWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setStyleSheet(QString::fromUtf8("#frame{\n"
"background-color: rgb(255, 255, 255);\n"
"}"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        addUser = new QPushButton(frame);
        addUser->setObjectName(QString::fromUtf8("addUser"));
        addUser->setGeometry(QRect(20, 20, 111, 71));
        addOk = new QPushButton(frame);
        addOk->setObjectName(QString::fromUtf8("addOk"));
        addOk->setGeometry(QRect(870, 20, 111, 71));

        verticalLayout->addWidget(frame);

        tableWidget = new QTableWidget(UserWidget);
        if (tableWidget->columnCount() < 1)
            tableWidget->setColumnCount(1);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setShowGrid(false);
        tableWidget->setColumnCount(1);
        tableWidget->horizontalHeader()->setDefaultSectionSize(150);
        tableWidget->horizontalHeader()->setHighlightSections(false);
        tableWidget->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(tableWidget);

        verticalLayout->setStretch(0, 2);
        verticalLayout->setStretch(1, 8);

        retranslateUi(UserWidget);

        QMetaObject::connectSlotsByName(UserWidget);
    } // setupUi

    void retranslateUi(QWidget *UserWidget)
    {
        UserWidget->setWindowTitle(QApplication::translate("UserWidget", "Form", 0, QApplication::UnicodeUTF8));
        addUser->setText(QApplication::translate("UserWidget", "\346\267\273\345\212\240\347\224\250\346\210\267", 0, QApplication::UnicodeUTF8));
        addOk->setText(QApplication::translate("UserWidget", "\345\256\214\346\210\220", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class UserWidget: public Ui_UserWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERWIDGET_H
