#ifndef DEVICESIMULATE_H
#define DEVICESIMULATE_H

#include <QWidget>
#include <QButtonGroup>

namespace Ui {
class DeviceSimulate;
}

class DeviceSimulate : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceSimulate(QWidget *parent = 0);
    ~DeviceSimulate();

private:
    Ui::DeviceSimulate *ui;
    QButtonGroup group_card;
    QButtonGroup group_drug;

    void initGroup();
    void initBtnText();

signals:
    void sendCardReaderData(QByteArray);
    void sendCodeScanData(QByteArray);
    void sendRfidData(QByteArray);

public slots:
    void recvLockCtrlData(QByteArray);

private slots:
    void group_card_clicked(int);
    void group_drug_clicked(int);
    void on_listCode_clicked();
    void on_listCode_2_clicked();
    void on_car1_clicked();
    void on_car2_clicked();
};

#endif // DEVICESIMULATE_H
