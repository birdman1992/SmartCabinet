#ifndef VERSIONINFO_H
#define VERSIONINFO_H
#include <QString>
#include <QFile>
#include "Json/cJSON.h"

class VersionInfo
{
public:
    VersionInfo(QString version);
    bool needUpdate(QByteArray qba);
    bool pacIsLegal(QFile* f);
    QString curVersion;
    QString serverVersion;
    QString pacFile;
    QString check;
    QString exe;
};

#endif // VERSIONINFO_H
