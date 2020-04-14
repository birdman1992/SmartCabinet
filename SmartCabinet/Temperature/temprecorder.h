#ifndef TEMPRECORDER_H
#define TEMPRECORDER_H
#include <QFile>
#include <QDateTime>
#include <QByteArray>
#include <QDir>

class TempRecorder
{
public:
    TempRecorder(QString id);
    ~TempRecorder();
    QString caseId;
    bool recordTemp(float wData, float sData, int state);
    QString recDir();

private:
    QFile* f;
    QString recordDir;
    QDate curDate;

    bool isNextDay();
    QString recordName();
};

#endif // TEMPRECORDER_H
