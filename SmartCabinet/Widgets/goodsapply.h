#ifndef GOODSAPPLY_H
#define GOODSAPPLY_H

#include <QWidget>

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
    void recvSearchRst();

signals:
    void searchRequire(QString);

private slots:
    void on_searchText_textChanged(const QString &arg1);

private:
    Ui::GoodsApply *ui;
};

#endif // GOODSAPPLY_H
