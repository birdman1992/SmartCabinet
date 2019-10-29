#ifndef FINGERSET_H
#define FINGERSET_H

#include <QWidget>
#include <QCanBus>

namespace Ui {
class FingerSet;
}

class FingerSet : public QWidget
{
    Q_OBJECT

public:
    explicit FingerSet(QWidget *parent = 0);
    ~FingerSet();

private:
    Ui::FingerSet *ui;
};

#endif // FINGERSET_H
