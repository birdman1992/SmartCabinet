#ifndef DAYREPORT_H
#define DAYREPORT_H

#include <QWidget>
#include <QPaintEvent>
#include <QEvent>
#include <QDate>
#include "Structs/dayreportinfo.h"

namespace Ui {
class DayReport;
}

class DayReport : public QWidget
{
    Q_OBJECT

public:
    explicit DayReport(QWidget *parent = 0);
    ~DayReport();

public slots:
    void recvReportInfo(QList<DayReportInfo*>, QString msg);

private slots:
    void on_date_start_dateChanged(const QDate &date);
    void on_date_finish_dateChanged(const QDate &date);
    void on_close_clicked();
    void on_check_clicked();

//    void on_list_table_id_cellClicked(int row, int column);

    void on_check_next_clicked();

signals:
    void askListInfo(QDate sDate, QDate eDate);

private:
    Ui::DayReport *ui;
//    QList<CheckTableInfo *> listCheckTables;
    QStringList listDates;
    bool eventFilter(QObject *, QEvent *);
//    void setCheckTables(QStringList);
    void setCurReportTable(QList<DayReportInfo*>l);
//    void showOnlyCheckTable(CheckList* l);
    QStringList createDateList(QDateTime sDate, QDateTime eDate);
};

#endif // DAYREPORT_H
