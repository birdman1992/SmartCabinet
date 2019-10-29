#ifndef TEMPDEV_H
#define TEMPDEV_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QLabel>

class TempDev : public QObject
{
    Q_OBJECT
public:
    explicit TempDev(QObject *parent = 0);
    void setTempLabel(QLabel* lab);//设置湿度文本标签,湿度文本会直接更新到文本标签
    void setHumLabel(QLabel* lab);//设置湿度文本标签,湿度文本会直接更新到文本标签

public slots:
    void recvTempData(QByteArray tempData);

signals:
    //recvTempData槽将触发温湿度更新信号
    void updateTempString(QString);//更新温度,字符串形式
    void updateHumString(QString);//更新湿度,字符串形式
    void updateTemp(float);//更新温度,字符串形式
    void updateHum(float);//更新湿度,字符串形式

private:
    int tempInteger;//温度：整数部分
    int tempDecimals;//温度：小数部分
    int humInteger;//湿度：整数部分
    int humDecimals;//湿度：小数部分
    QLabel* tempLabel;
    QLabel* humLabel;

private:
    QString dataToString(int dataInteger, int dataDecimals, QString unit);
};

#endif // TEMPDEV_H
