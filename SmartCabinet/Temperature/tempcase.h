#ifndef TEMPCASE_H
#define TEMPCASE_H

#include <QWidget>
#include <QPaintEvent>
#include <qpainter.h>
#include <QTcpSocket>
#include <QByteArray>
#include <QString>
#include "tempmanager.h"
#include "temprecorder.h"
#include <QVector>
#include <QMouseEvent>
#include <QDateTime>
#include <QTimer>

namespace Ui {
class TempCase;
}
class TempManager;

class TempCase : public QWidget
{
    Q_OBJECT

public:
    explicit TempCase(QWidget *parent = 0);
//    TempCase(QWidget *parent = 0, float maxTemp=100, float minTemp=-273.15);
    ~TempCase();
    void updateTemp(float temp);
    void updateHum(float hum);
    float getCurTemp();
    float getCurHum();
    int getCurState();
    QString getCurStateStr();
    void setSocket(QTcpSocket* t);
    void setTempParams(int _max, int _min, int _warning);
    QString devId();
    QString devColor();
    QString devIp();
    QString devName();
    QString getCaseName(QString id);
    void setCaseName(QString name);
    int maxTemp();
    int minTemp();
    int warningTemp();
    bool creatHistoryData(QDate startDate, int sampleNum);
    void clearHistoryData();
    QVector<double> dataTime();
    QVector<double> dataHum();
    QVector<double> dataTemp();
    void startSet();
    void checkOverTime();

private:
    Ui::TempCase *ui;
    QTcpSocket* dev;
    TempManager* devManager;
    TempRecorder* recorder;
    QTime lastRecordTime;
    QDateTime overTime;
    QMap<QString, QString> nameMap;
    QVector<double> temps;
    QVector<double> hums;
    QVector<double> times;
    void setCaseState(int state);
    int curState;
    int recordCount;
    QString caseId;
    QString caseName;
    int tReport;
    int tMax;
    int tMin;
    int tWarning;
    float tCur;
    float hCur;
    void setCaseId(QString id);
    unsigned char int2Byte(int val);
    QByteArray getParamsBytes();
    float parTemp(QByteArray qba);

    float parHum(QByteArray qba);
    void paintEvent(QPaintEvent *);
    void updateOverTime();
    void initNameMap();


protected:
    void mouseReleaseEvent(QMouseEvent *);
signals:
    void caseIdUpdate(TempCase*);
    void caseClicked(TempCase*);

private slots:
    void setDevParam();
    void recvDevData();
    void devStateChanged(QAbstractSocket::SocketState);
    void on_save_clicked(bool checked);
};

#endif // TEMPCASE_H
