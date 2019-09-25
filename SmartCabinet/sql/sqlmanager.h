#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#define DB_CABINET "Cabinet"

class SqlManager : public QObject
{
    Q_OBJECT
public:
    explicit SqlManager(QObject *parent = 0);
    ~SqlManager();

public slots:

signals:

private:
    QSqlDatabase db_cabinet;

    void initDatabase();
    void createTable();

};

#endif // SQLMANAGER_H
