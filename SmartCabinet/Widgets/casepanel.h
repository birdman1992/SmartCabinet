#ifndef CASEPANEL_H
#define CASEPANEL_H

#include <QWidget>
#include <QStringList>
#include <QFont>
#include <QResizeEvent>
#include "Structs/cabinetinfo.h"

namespace Ui {
class CasePanel;
}

class CasePanel : public QWidget
{
    Q_OBJECT

public:
    explicit CasePanel(bool doubleCol ,QWidget *parent = 0);
    ~CasePanel();
    void setCheckState(bool checked);
    void setText(QStringList text);
    void setText(QList<Goods*> list);
    QFont caseFont();
    int labWidth();
    int maxShowNum();
    void setSpec(bool spec);
    bool isSpecialCase();

private:
    Ui::CasePanel *ui;
    bool showDoubleCol;
    bool isSpec;//特殊柜
    QFont* font;
    QStringList cur_show;
    QList<Goods *> list_show;
    QString geteElidedText(QFont _font, QString str, int MaxWidth);
    QString getShowStr(Goods* info);
    QString getShowStr(QString goodsStr);
    int getMaxLine();
    int getStringWidth(QString str);
    void updatePanel();
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent* );
    QString cellStyle(QColor rgb);
};

#endif // CASEPANEL_H
