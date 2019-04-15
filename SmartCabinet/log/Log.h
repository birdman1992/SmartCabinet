#ifndef LOG_H
#define LOG_H
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>
#include "log4qt/logger.h"
#include "log4qt/basicconfigurator.h"

class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = NULL);

signals:

public slots:
    static Log * instance();

public slots:
    void init(QString configFilePath);

public slots:
    void debug(QString msg);
    void info(QString msg);
    void warn(QString msg);
    void error(QString msg);

private:
    static Log *_pInstance;
    static QMutex _mutex;
    static Log4Qt::Logger * _pLoggerDebug;
    static Log4Qt::Logger * _pLoggerInfo;
    static Log4Qt::Logger * _pLoggerWarn;
    static Log4Qt::Logger * _pLoggerError;
    static QString _configFilePath;
};
#endif
