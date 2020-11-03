#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QShowEvent>
#include <QPainter>
#include <QString>
#include <manager/singleton.h>

namespace Ui {
class MessageDialog;
}

class MessageDialog : public QDialog, public Singleton<MessageDialog>
{
    Q_OBJECT
    friend class Singleton<MessageDialog>;

public:
    void showMessage(QString msg, int showS=0);
    void showFinish();

protected:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent* e);
    void showEvent(QShowEvent*);


private slots:
    void on_close_clicked();

private:
    Ui::MessageDialog *ui;
    explicit MessageDialog(QWidget *parent = nullptr);
    ~MessageDialog();
    void updateMessage();

    QString message;
    int timerId;
    int curCount;
    int downCount;
};

#endif // MESSAGEDIALOG_H
