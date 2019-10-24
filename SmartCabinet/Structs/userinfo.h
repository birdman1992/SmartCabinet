#ifndef USERINFO_H
#define USERINFO_H
#include <QString>

class UserInfo
{
public:
    UserInfo();
    QString name;
//    int id;
    QString cardId;
    QString departId;
    int power;//0 超级管理员, 1 仓库员工, 2 医院管理, 3 医院员工
    QString identityId;
    QString tel;
};

#endif // USERINFO_H
