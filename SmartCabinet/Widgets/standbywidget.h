#ifndef STANDBYWIDGET_H
#define STANDBYWIDGET_H

#include <QWidget>

namespace Ui {
class StandbyWidget;
}

class StandbyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StandbyWidget(QWidget *parent = 0);
    ~StandbyWidget();

private:
    Ui::StandbyWidget *ui;
};

#endif // STANDBYWIDGET_H
