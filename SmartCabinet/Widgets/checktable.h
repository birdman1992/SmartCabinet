#ifndef CHECKTABLE_H
#define CHECKTABLE_H

#include <QWidget>
#include <QPaintEvent>
#include <QEvent>
#include "Structs/goodscheckinfo.h"

namespace Ui {
class CheckTable;
}

class CheckTable : public QWidget
{
    Q_OBJECT

public:
    explicit CheckTable(QWidget *parent = 0);
    ~CheckTable();

public slots:
    void updateCheckTable(CheckList* l);
    void recvCheckTables(QList<CheckTableInfo*>);

private slots:
    void on_date_start_dateChanged(const QDate &date);
    void on_date_finish_dateChanged(const QDate &date);
    void on_close_clicked();
    void on_check_clicked();

    void on_list_table_id_cellClicked(int row, int column);

signals:
    void askCheckTables(QDate start, QDate finish);
    void askCheckInfo(QString id);

private:
    Ui::CheckTable *ui;
    QList<CheckTableInfo *> listCheckTables;
    bool eventFilter(QObject *, QEvent *);
    void setCheckTables(QStringList);
    void setCurCheckTable(CheckList*l);
    void showOnlyCheckTable(CheckList* l);
};

#endif // CHECKTABLE_H
