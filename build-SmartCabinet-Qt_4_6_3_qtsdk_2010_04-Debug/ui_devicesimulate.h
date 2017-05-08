/********************************************************************************
** Form generated from reading UI file 'devicesimulate.ui'
**
** Created: Mon May 8 11:00:55 2017
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICESIMULATE_H
#define UI_DEVICESIMULATE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DeviceSimulate
{
public:
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;
    QWidget *cardReader;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QWidget *codeScan;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    QPushButton *pushButton_11;
    QPushButton *pushButton_12;
    QPushButton *pushButton_13;
    QPushButton *pushButton_14;
    QPushButton *pushButton_15;
    QPushButton *pushButton_16;
    QPushButton *pushButton_17;
    QPushButton *pushButton_18;
    QPushButton *pushButton_19;
    QPushButton *pushButton_20;
    QPushButton *pushButton_21;
    QPushButton *pushButton_22;
    QPushButton *pushButton_23;
    QPushButton *pushButton_24;
    QWidget *lockCtrl;

    void setupUi(QWidget *DeviceSimulate)
    {
        if (DeviceSimulate->objectName().isEmpty())
            DeviceSimulate->setObjectName(QString::fromUtf8("DeviceSimulate"));
        DeviceSimulate->setWindowModality(Qt::NonModal);
        DeviceSimulate->resize(640, 480);
        DeviceSimulate->setStyleSheet(QString::fromUtf8("#DeviceSimulate{\n"
" background-color: rgb(255, 255, 255);\n"
"}"));
        verticalLayout = new QVBoxLayout(DeviceSimulate);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        listWidget = new QListWidget(DeviceSimulate);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setTextElideMode(Qt::ElideRight);
        listWidget->setFlow(QListView::LeftToRight);

        verticalLayout->addWidget(listWidget);

        stackedWidget = new QStackedWidget(DeviceSimulate);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        cardReader = new QWidget();
        cardReader->setObjectName(QString::fromUtf8("cardReader"));
        pushButton = new QPushButton(cardReader);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(70, 60, 131, 51));
        pushButton->setFocusPolicy(Qt::NoFocus);
        pushButton_2 = new QPushButton(cardReader);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(70, 130, 131, 51));
        pushButton_2->setFocusPolicy(Qt::NoFocus);
        pushButton_3 = new QPushButton(cardReader);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(70, 200, 131, 51));
        pushButton_3->setFocusPolicy(Qt::NoFocus);
        pushButton_4 = new QPushButton(cardReader);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(70, 270, 131, 51));
        pushButton_4->setFocusPolicy(Qt::NoFocus);
        stackedWidget->addWidget(cardReader);
        codeScan = new QWidget();
        codeScan->setObjectName(QString::fromUtf8("codeScan"));
        pushButton_5 = new QPushButton(codeScan);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(40, 40, 131, 51));
        pushButton_5->setFocusPolicy(Qt::NoFocus);
        pushButton_6 = new QPushButton(codeScan);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(40, 110, 131, 51));
        pushButton_6->setFocusPolicy(Qt::NoFocus);
        pushButton_7 = new QPushButton(codeScan);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(40, 180, 131, 51));
        pushButton_7->setFocusPolicy(Qt::NoFocus);
        pushButton_8 = new QPushButton(codeScan);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(40, 250, 131, 51));
        pushButton_8->setFocusPolicy(Qt::NoFocus);
        pushButton_9 = new QPushButton(codeScan);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(40, 320, 131, 51));
        pushButton_9->setFocusPolicy(Qt::NoFocus);
        pushButton_10 = new QPushButton(codeScan);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));
        pushButton_10->setGeometry(QRect(180, 320, 131, 51));
        pushButton_10->setFocusPolicy(Qt::NoFocus);
        pushButton_11 = new QPushButton(codeScan);
        pushButton_11->setObjectName(QString::fromUtf8("pushButton_11"));
        pushButton_11->setGeometry(QRect(180, 180, 131, 51));
        pushButton_11->setFocusPolicy(Qt::NoFocus);
        pushButton_12 = new QPushButton(codeScan);
        pushButton_12->setObjectName(QString::fromUtf8("pushButton_12"));
        pushButton_12->setGeometry(QRect(180, 250, 131, 51));
        pushButton_12->setFocusPolicy(Qt::NoFocus);
        pushButton_13 = new QPushButton(codeScan);
        pushButton_13->setObjectName(QString::fromUtf8("pushButton_13"));
        pushButton_13->setGeometry(QRect(180, 110, 131, 51));
        pushButton_13->setFocusPolicy(Qt::NoFocus);
        pushButton_14 = new QPushButton(codeScan);
        pushButton_14->setObjectName(QString::fromUtf8("pushButton_14"));
        pushButton_14->setGeometry(QRect(180, 40, 131, 51));
        pushButton_14->setFocusPolicy(Qt::NoFocus);
        pushButton_15 = new QPushButton(codeScan);
        pushButton_15->setObjectName(QString::fromUtf8("pushButton_15"));
        pushButton_15->setGeometry(QRect(320, 320, 131, 51));
        pushButton_15->setFocusPolicy(Qt::NoFocus);
        pushButton_16 = new QPushButton(codeScan);
        pushButton_16->setObjectName(QString::fromUtf8("pushButton_16"));
        pushButton_16->setGeometry(QRect(320, 180, 131, 51));
        pushButton_16->setFocusPolicy(Qt::NoFocus);
        pushButton_17 = new QPushButton(codeScan);
        pushButton_17->setObjectName(QString::fromUtf8("pushButton_17"));
        pushButton_17->setGeometry(QRect(320, 250, 131, 51));
        pushButton_17->setFocusPolicy(Qt::NoFocus);
        pushButton_18 = new QPushButton(codeScan);
        pushButton_18->setObjectName(QString::fromUtf8("pushButton_18"));
        pushButton_18->setGeometry(QRect(320, 110, 131, 51));
        pushButton_18->setFocusPolicy(Qt::NoFocus);
        pushButton_19 = new QPushButton(codeScan);
        pushButton_19->setObjectName(QString::fromUtf8("pushButton_19"));
        pushButton_19->setGeometry(QRect(320, 40, 131, 51));
        pushButton_19->setFocusPolicy(Qt::NoFocus);
        pushButton_20 = new QPushButton(codeScan);
        pushButton_20->setObjectName(QString::fromUtf8("pushButton_20"));
        pushButton_20->setGeometry(QRect(460, 320, 131, 51));
        pushButton_20->setFocusPolicy(Qt::NoFocus);
        pushButton_21 = new QPushButton(codeScan);
        pushButton_21->setObjectName(QString::fromUtf8("pushButton_21"));
        pushButton_21->setGeometry(QRect(460, 180, 131, 51));
        pushButton_21->setFocusPolicy(Qt::NoFocus);
        pushButton_22 = new QPushButton(codeScan);
        pushButton_22->setObjectName(QString::fromUtf8("pushButton_22"));
        pushButton_22->setGeometry(QRect(460, 250, 131, 51));
        pushButton_22->setFocusPolicy(Qt::NoFocus);
        pushButton_23 = new QPushButton(codeScan);
        pushButton_23->setObjectName(QString::fromUtf8("pushButton_23"));
        pushButton_23->setGeometry(QRect(460, 110, 131, 51));
        pushButton_23->setFocusPolicy(Qt::NoFocus);
        pushButton_24 = new QPushButton(codeScan);
        pushButton_24->setObjectName(QString::fromUtf8("pushButton_24"));
        pushButton_24->setGeometry(QRect(460, 40, 131, 51));
        pushButton_24->setFocusPolicy(Qt::NoFocus);
        stackedWidget->addWidget(codeScan);
        lockCtrl = new QWidget();
        lockCtrl->setObjectName(QString::fromUtf8("lockCtrl"));
        stackedWidget->addWidget(lockCtrl);

        verticalLayout->addWidget(stackedWidget);

        verticalLayout->setStretch(0, 80);
        verticalLayout->setStretch(1, 800);

        retranslateUi(DeviceSimulate);

        stackedWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(DeviceSimulate);
    } // setupUi

    void retranslateUi(QWidget *DeviceSimulate)
    {
        DeviceSimulate->setWindowTitle(QApplication::translate("DeviceSimulate", "Form", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("DeviceSimulate", "00000001", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("DeviceSimulate", "00000002", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("DeviceSimulate", "00000003", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("DeviceSimulate", "00000004", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201A", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201B", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201C", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201D", 0, QApplication::UnicodeUTF8));
        pushButton_9->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201E", 0, QApplication::UnicodeUTF8));
        pushButton_10->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201J", 0, QApplication::UnicodeUTF8));
        pushButton_11->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201H", 0, QApplication::UnicodeUTF8));
        pushButton_12->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201I", 0, QApplication::UnicodeUTF8));
        pushButton_13->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201G", 0, QApplication::UnicodeUTF8));
        pushButton_14->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201F", 0, QApplication::UnicodeUTF8));
        pushButton_15->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201O", 0, QApplication::UnicodeUTF8));
        pushButton_16->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201M", 0, QApplication::UnicodeUTF8));
        pushButton_17->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201N", 0, QApplication::UnicodeUTF8));
        pushButton_18->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201L", 0, QApplication::UnicodeUTF8));
        pushButton_19->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201K", 0, QApplication::UnicodeUTF8));
        pushButton_20->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201T", 0, QApplication::UnicodeUTF8));
        pushButton_21->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201R", 0, QApplication::UnicodeUTF8));
        pushButton_22->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201S", 0, QApplication::UnicodeUTF8));
        pushButton_23->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201Q", 0, QApplication::UnicodeUTF8));
        pushButton_24->setText(QApplication::translate("DeviceSimulate", "\350\215\257\345\223\201P", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DeviceSimulate: public Ui_DeviceSimulate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICESIMULATE_H
