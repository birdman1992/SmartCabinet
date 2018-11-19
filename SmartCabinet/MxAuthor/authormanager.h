#ifndef AUTHORMANAGER_H
#define AUTHORMANAGER_H
#include <QFile>

class AuthorManager
{
public:
    AuthorManager();
    void authorStart();
    bool authorCheck();

private:
    QByteArray getAuthorSrc();
};

#endif // AUTHORMANAGER_H
