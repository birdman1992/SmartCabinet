#include "operation.h"
#include "ui_operation.h"
#include <QDebug>
#include <QString>

Operation::Operation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Operation)
{
    ui->setupUi(this);
    optListModel = new QSqlQueryModel;
    ui->view_operation->setModel(optListModel);
    ui->view_operation->setModelColumn(0);

    optInfoModel = new QSqlQueryModel;
    ui->view_operation_goods->setModel(optInfoModel);
    setViewStyle();

    lab_opt_info_list<<ui->surgery_bill_no
                    <<ui->patient_name
                   <<ui->patient_gender
                  <<ui->patient_age
                 <<ui->patient_no
                <<ui->surgery_order_no
               <<ui->surgery_bill_name
              <<ui->operating_table
             <<ui->apply_depot_name
            <<ui->apply_doctor_name
           <<ui->apply_surgery_date
          <<ui->exec_depot_name
         <<ui->exec_doctor_name
        <<ui->exec_surgery_date;
}

Operation::~Operation()
{
    delete ui;
}

QString Operation::curOperationStr()
{
    return QString("");
}

QString Operation::curOperationNo() const
{
    return m_curOperationNo;
}

void Operation::loadOperations()
{
    qDebug()<<"[loadOperations]";
    QString queryStr = QString("select patient_name,ssc_surgery_bill_id from OperationInfo;");
    optListModel->setQuery(SqlManager::query(queryStr, "[loadOperations]"));
}

void Operation::setCurOperationNo(QString curOperationNo)
{
    if (m_curOperationNo == curOperationNo)
        return;

    ui->lab_curOperation->setText(curOperationNo);
    m_curOperationNo = curOperationNo;
    emit CurOperationNoChanged(m_curOperationNo);
}

void Operation::setCurOperationStr(QString curOperationStr)
{
    if (m_curOperationStr == curOperationStr)
        return;

    m_curOperationStr = curOperationStr;
    emit curOperationStrChanged(m_curOperationStr);
    if(curOperationStr.isEmpty())
    {
        foreach (QLabel* lab, lab_opt_info_list)
        {
            lab->clear();
        }
        optInfoModel->clear();
    }
}

void Operation::on_back_clicked()
{
    emit winClose();
}

void Operation::on_update_clicked()
{
    emit requireUpdate();
}

void Operation::showEvent(QShowEvent *)
{
    loadOperations();
    emit requireUpdate();
}

void Operation::setViewStyle()
{
    //    ui->view_operation->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->view_operation->setAlternatingRowColors(true);
    ui->view_operation->setStyleSheet("color: rgb(0, 0, 0);    /*前景色：文字颜色*/"
                                      "background:white;"
                                      "gridline-color:rgb(161,161,161);"
                                      "alternate-background-color:rgb(244, 244, 244);"
                                      "selection-color:white;    /*鼠标选中时前景色：文字颜色*/"
                                      "selection-background-color:rgb(23, 166, 255);   /*鼠标选中时背景色*/");

//    ui->view_operation_goods->setColumnWidth(0);
    ui->view_operation_goods->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->view_operation_goods->setAlternatingRowColors(true);
    ui->view_operation_goods->setStyleSheet("color: rgb(0, 0, 0);    /*前景色：文字颜色*/"
                                            "background:white;"
                                            "gridline-color:rgb(161,161,161);"
                                            "alternate-background-color:rgb(244, 244, 244);"
                                            "selection-color:white;    /*鼠标选中时前景色：文字颜色*/"
                                            "selection-background-color:rgb(23, 166, 255);   /*鼠标选中时背景色*/");

    QFile qssScrollbar(":/stylesheet/styleSheet/ScrollBar.qss");
    qssScrollbar.open(QIODevice::ReadOnly);
    QString style = QString(qssScrollbar.readAll());
    ui->frame->setStyleSheet(style);
    qssScrollbar.close();
}

void Operation::selCurOperation(QString optId)
{
    QSqlQuery query = SqlManager::query(QString("select "
                      "surgery_bill_no,"
                      "patient_name,"
                      "patient_gender,"
                      "patient_age,"
                      "patient_no,"
                      "surgery_order_no,"
                      "surgery_bill_name,"
                      "operating_table,"
                      "apply_depot_name,"
                      "apply_doctor_name,"
                      "apply_surgery_date,"
                      "exec_depot_name,"
                      "exec_doctor_name,"
                      "exec_surgery_date,"
                      "ssc_surgery_bill_id "
                      "from OperationInfo "
                      "where ssc_surgery_bill_id='%1'").arg(optId)
                      , "[selCurOperation]");

    if(query.next())
    {
        for(int i=0; i<lab_opt_info_list.count(); i++)
        {
            qDebug()<<query.value(i).toString();
            lab_opt_info_list[i]->setText(query.value(i).toString());
        }
    }

    setCurOperationNo(ui->surgery_bill_no->text());
    setCurOperationStr(QString("手术单:%1 %2").arg(ui->surgery_bill_no->text()).arg(ui->surgery_bill_name->text()));
}

void Operation::on_view_operation_clicked(const QModelIndex &index)
{
    qDebug()<<index.row()<<index.column();
    QModelIndex idx = ui->view_operation->model()->index(index.row(), 1);
    QString optId = ui->view_operation->model()->itemData(idx).value(0).toString();
    selCurOperation(optId);
}
