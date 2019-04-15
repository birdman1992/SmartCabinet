#include "Log.h"
#include "log4qt/log4qt.h"
#include "log4qt/propertyconfigurator.h"

Log * Log::_pInstance = 0;
QMutex Log::_mutex;
Log4Qt::Logger * Log::_pLoggerDebug = 0;
Log4Qt::Logger * Log::_pLoggerInfo = 0;
Log4Qt::Logger * Log::_pLoggerWarn = 0;
Log4Qt::Logger * Log::_pLoggerError = 0;
QString Log::_configFilePath;

Log::Log(QObject *parent) : QObject(parent)
{
    // 一定要配置文件，不然运行起来会直接当掉
    Log4Qt::BasicConfigurator::configure();
}

Log *Log::instance()
{
    if(!_pInstance)
    {
        QMutexLocker mutexLocker(&_mutex);
        if(!_pInstance)
        {
            Log *pInstance = new Log();
            _pInstance = pInstance;
        }
    }
    return _pInstance;
}

void Log::init(QString configFilePath)
{
    _configFilePath = configFilePath;
     Log4Qt::PropertyConfigurator::configure(_configFilePath);
    _pLoggerDebug = Log4Qt::Logger::logger("debug");
    _pLoggerInfo = Log4Qt::Logger::logger("info");
    _pLoggerWarn = Log4Qt::Logger::logger("warn");
    _pLoggerError = Log4Qt::Logger::logger("error");
}

void Log::debug(QString msg)
{
    _pLoggerDebug->debug(msg);
}

void Log::info(QString msg)
{
    _pLoggerInfo->info(msg);
}
void Log::warn(QString msg)
{
    _pLoggerWarn->warn(msg);
}

void Log::error(QString msg)
{
    _pLoggerError->error(msg);
}
