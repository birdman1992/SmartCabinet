#ifndef CABINETLISTITEM_H
#define CABINETLISTITEM_H

#include <QWidget>

namespace Ui {
class CabinetListItem;
}

class CabinetListItem : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetListItem(QWidget *parent = 0);
    ~CabinetListItem();

private:
    Ui::CabinetListItem *ui;
};

#endif // CABINETLISTITEM_H
