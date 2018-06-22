#ifndef CHECKWARNING_H
#define CHECKWARNING_H

#include <QWidget>
#include <QPaintEvent>

namespace Ui {
class CheckWarning;
}

class CheckWarning : public QWidget
{
    Q_OBJECT

public:
    explicit CheckWarning(QWidget *parent = 0);
    ~CheckWarning();

signals:
    void pushCheck();

public slots:
    void warnningMsg(QString msg, bool pushReady);
    void checkSuccess(bool success);

private slots:
    void on_back_clicked();
    void on_push_clicked();

private:
    Ui::CheckWarning *ui;
    void paintEvent(QPaintEvent *);
};

#endif // CHECKWARNING_H
