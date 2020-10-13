#ifndef TEMPTOPDF_H
#define TEMPTOPDF_H

#include <QObject>
#include <QThread>
#include <QPrinter>
#include <QPainter>
#include <QApplication>
#include <QDate>
#include "tempmanager.h"
#include "config.h"
#include "cabinetconfig.h"

class TempToPdf : public QThread
{
    Q_OBJECT
public:
    explicit TempToPdf(QObject *parent = 0);
    QByteArray getDownloadPath();

private:
    void run();

    TempManager* tManager;
    QString appPath;
    QString logPath;
    QString outputPath;
    QString downLoadPath;
    QString recordName;
    QStringList weekList;
    QStringList stateList;
    bool emptyTask;
    int taskNum;
    int taskProgress;
    bool creatTempPdf(QString dev, QDate beginDate, QDate endDate);
    void pdfCreateFinish();
signals:
    void updateProgress(int progress);
    void taskFinish();

public slots:
    void creatTempPdf(QStringList devList, QDate beginDate, QDate endDate);
};

#endif // TEMPTOPDF_H
