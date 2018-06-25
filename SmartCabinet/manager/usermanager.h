#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QStringList>

class UserInfo
{
public:
    UserInfo();
    QString card_no;
    QString real_name;
    QString role_id;
    QString role_name;
};


class UserManager : public QObject
{
    Q_OBJECT
public:
    static UserManager* manager();
    /*GETS*/
    UserInfo* getUserInfo(QString card_no);

    /*SETS*/
    void setUserInfo(UserInfo* info);

private:
    explicit UserManager(QObject *parent = NULL);
    static UserManager* m;

    void removeConfig(QString path);
    QVariant getConfig(QString key, QVariant defaultRet);
    QStringList getConfigGroups();
    void setConfig(QString key, QVariant value);
signals:

public slots:
};

#endif // USERMANAGER_H
