#ifndef CABINETLISTITEM_H
#define CABINETLISTITEM_H

#include <QWidget>
#include <QString>

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
    QString name;
    QString pack_id;//包id:xxxxxx-xx
    QStringList code_bar;//条形码列表:xxxxxx-xxxxxx-xx-xxxx
    int num;
};

#endif // CABINETLISTITEM_H
