#ifndef GOODSAPPLY_H
#define GOODSAPPLY_H

#include <QWidget>
#include "Structs/goodscheckinfo.h"
#include "Widgets/keyboad.h"

namespace Ui {
class GoodsApply;
}

class GoodsApply : public QWidget
{
    Q_OBJECT

public:
    explicit GoodsApply(QWidget *parent = 0);
    ~GoodsApply();

public slots:
    void recvSearchRst(CheckList* l);
    void recvReplyRst(bool success, QString msg);
    void show();

signals:
    void searchRequire(QString);
    void replyRequire(QList<GoodsCheckInfo*> l);

private slots:
    void keyboadStr(QString);
    void on_searchText_textChanged(const QString &arg1);
    void on_searchRst_cellClicked(int row, int column);
    void on_replyTable_cellClicked(int row, int column);
    void on_push_clicked();
    void on_pushButton_clicked(bool checked);

    void on_close_clicked();

private:
    Ui::GoodsApply *ui;
    Keyboad* keyboad;
    QList<GoodsCheckInfo*> listSearch;
    QList<GoodsCheckInfo*> listPush;

    void setTableSearch(QList<GoodsCheckInfo*> l);
    void pushTableInsert(GoodsCheckInfo* info);
    void pushTableDelete(int row);
    void updatePushTableRow(int row, GoodsCheckInfo* info);
    bool isRepeat(QList<GoodsCheckInfo*>l, GoodsCheckInfo* info);
};

#endif // GOODSAPPLY_H
