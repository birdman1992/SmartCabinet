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
    explicit CasePanel(QWidget *parent = 0);
    ~CasePanel();
    void setCheckState(bool checked);
    void setText(QStringList text);
    void setText(QList<GoodsInfo*> list);
    QFont caseFont();
    int labWidth();

private:
    Ui::CasePanel *ui;
    QFont* font;
    QList<GoodsInfo *> list_show;
    QString geteElidedText(QFont _font, QString str, int MaxWidth);
    QString getShowStr(GoodsInfo* info);
    int getMaxLine();
    int getStringWidth(QString str);
    void updatePanel();
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent* );
};

#endif // CASEPANEL_H
