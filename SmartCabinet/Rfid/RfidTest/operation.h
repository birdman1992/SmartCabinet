#ifndef OPERATION_H
#define OPERATION_H

#include <QWidget>
#include <QShowEvent>
#include <QSqlQueryModel>
#include <QLabel>
#include <sql/sqlmanager.h>

namespace Ui {
class Operation;
}

class Operation : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString curOperationNo READ curOperationNo WRITE setCurOperationNo NOTIFY CurOperationNoChanged)
    Q_PROPERTY(QString curOperationStr READ curOperationStr WRITE setCurOperationStr NOTIFY curOperationStrChanged)

public:
    explicit Operation(QWidget *parent = nullptr);
    ~Operation();

    QString curOperationStr();//当前手术单
    QString curOperationNo() const;


public slots:
    void loadOperations();//加载手术单

    void setCurOperationNo(QString curOperationNo);

    void setCurOperationStr(QString curOperationStr);

signals:
    void requireUpdate();
    void updateSelOperation(QString info);//刷新当前选择手术单信息
    void winClose();

    void CurOperationNoChanged(QString curOperationNo);

    void curOperationStrChanged(QString curOperationStr);

private slots:
    void on_back_clicked();//返回
    void on_update_clicked();//刷新

    void on_view_operation_clicked(const QModelIndex &index);

private:
    Ui::Operation *ui;
    QSqlQueryModel* optListModel;
    QSqlQueryModel* optInfoModel;
    QList<QLabel*> lab_opt_info_list;

    void showEvent(QShowEvent*);
    void setViewStyle();
    void selCurOperation(QString optId);

    QString m_curOperationNo;

    QString m_curOperationStr;
};

#endif // OPERATION_H
