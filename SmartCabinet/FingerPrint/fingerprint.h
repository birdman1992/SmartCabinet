#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <QWidget>
#include <QByteArray>
#include "qsocketcan.h"
#include "cmdpack.h"

namespace Ui {
class FingerPrint;
}

class FingerPrint : public QWidget
{
    Q_OBJECT

public:
    explicit FingerPrint(QWidget *parent = 0);
    ~FingerPrint();

signals:
    void requireOpenLock(int seq, int index);

private slots:
    void moduleActived(int id);
    void on_test_clicked();

    void on_test_upload_clicked();

    void on_test_download_clicked();

private:
    Ui::FingerPrint *ui;
    QSocketCan* socketCan;
};

#endif // FINGERPRINT_H
