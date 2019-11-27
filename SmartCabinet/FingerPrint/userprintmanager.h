#ifndef USERPRINTMANAGER_H
#define USERPRINTMANAGER_H
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QByteArray>
#include <QStringList>

class UserPrintManager
{
public:
    static UserPrintManager* manager();
    int getNewUserID(QString cardId);//获取一个新用户ID
    void setUserInfo(int userId, QString cardId, QString name, QByteArray fingerData);
    void setCtrlConfig(QByteArray confSeq, QByteArray confIndex);
    int getFingerId(QString cardId);
    QByteArray getCardId(int fingerId);
    QByteArray getSeqConfig();
    QByteArray getIndexConfig();
//    void addUser(QByteArray tempData,);

private:
    UserPrintManager();
    static UserPrintManager* m;
};

#endif // USERPRINTMANAGER_H
