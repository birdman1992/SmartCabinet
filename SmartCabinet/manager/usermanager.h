#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
class UserInfo
{
public:
    QString name;

};


class UserManager : public QObject
{
    Q_OBJECT
public:
    static UserManager* manager();
private:
    explicit UserManager(QObject *parent = NULL);
    static UserManager* m;
signals:

public slots:
};

#endif // USERMANAGER_H
