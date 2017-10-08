#ifndef CABINETLISTITEM_H
#define CABINETLISTITEM_H

#include <QWidget>
#include <QString>
#include <QPaintEvent>

namespace Ui {
class CabinetListItem;
}

class CabinetListItem : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetListItem(QString goodsName, QString goodsId, QWidget *parent = 0);
    ~CabinetListItem();
    bool addPackage(QString bar);//添加新扫描的条码
    QStringList getBarList();
    QString id();

private:
    Ui::CabinetListItem *ui;
    QString name;
    QString pack_id;//包id:xxxxxx-xx
    QStringList code_bar;//条形码列表:xxxxxx-xxxxxx-xx-xxxx
    int num;
    QString scanDataTrans(QString code);
    void paintEvent(QPaintEvent *);
};

#endif // CABINETLISTITEM_H
