#ifndef USERPRINTMANAGER_H
#define USERPRINTMANAGER_H


class UserPrintManager
{
public:
    static UserPrintManager* manager();
    void addUserImg(int );

private:
    UserPrintManager();
    static UserPrintManager* m;
};

#endif // USERPRINTMANAGER_H
