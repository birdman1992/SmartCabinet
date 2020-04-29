#ifndef TEMPCASE_H
#define TEMPCASE_H

#include <QWidget>
#include <QPaintEvent>
#include <qpainter.h>
#include <QTcpSocket>
#include <QByteArray>
#include <QString>
#include <QVector>
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

enum DevType{
    dev_x_1 = 1,//小屏幕
    dev_x_4 = 2,//大屏幕,4联
};

class TempCase : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(DevType tempDevType READ tempDevType WRITE setTempDevType)
    Q_PROPERTY(QString caseName READ caseName WRITE setCaseName)
    Q_PROPERTY(QString caseId READ caseId WRITE setCaseId)

public:
    enum DevState{
        dev_normal,
        dev_temp_over,
        dev_temp_under,
        dev_hum_over,
        dev_hum_under,
        dev_offline,
    };

public:
    explicit TempCase(QWidget *parent = 0);
//    TempCase(QWidget *parent = 0, float maxTemp=100, float minTemp=-273.15);
    ~TempCase();
    void updateTemp(QVector<float> temp);
    void updateHum(QVector<float> hum);
    float getCurTemp();
    float getCurHum();
    DevState getCurState();
//    QString getCurStateStr();
    void setSocket(QTcpSocket* t);
    void setTempParams(int _max, int _min, int _warningm, int _report);
    QString devId();
    QString devColor();
    QString devIp();
    QString devName();
    void setCaseName(QString name);
    int maxTemp();
    int minTemp();
    int warningTemp();
    int reportTime();
    bool creatHistoryData(QDate startDate, int sampleNum);
    void clearHistoryData();
    QVector<double> dataTime();
    QVector<double> dataHum();
    QVector<double> dataTemp();
    void startSet();
    void checkOverTime();

    DevType tempDevType() const
    {
        return m_tempDevType;
    }

    QString caseName() const
    {
        return m_caseName;
    }

    QString caseId() const
    {
        return m_caseId;
    }

public slots:
    void setTempDevType(DevType tempDevType)
    {
        m_tempDevType = tempDevType;
    }

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
    void setCaseState(DevState state);

    DevState curState;
    int recordCount;
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
    void pacBack(int8_t ctrlWd, int8_t errWd);//协议返回
    void parseLogin(QByteArray qba);
    void parseReport(QByteArray qba);
    QString pacToName(QByteArray qba);
    QByteArray nameToPac(QString name);

    DevType m_tempDevType;
    QString m_caseName;
    QString m_caseId;

protected:
    void mouseReleaseEvent(QMouseEvent *);
signals:
    void caseIdUpdate(TempCase*);
    void caseClicked(TempCase*);
    void caseReport(QString name, float temp, float hum);

private slots:
    void setDevParam();
    void recvDevData();
    void devStateChanged(QAbstractSocket::SocketState);
    void on_save_clicked(bool checked);
};

#endif // TEMPCASE_H
