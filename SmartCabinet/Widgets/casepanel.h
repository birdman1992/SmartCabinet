#ifndef CASEPANEL_H
#define CASEPANEL_H

#include <QWidget>
#include <QStringList>
#include <QFont>

namespace Ui {
class CasePanel;
}

class CasePanel : public QWidget
{
    Q_OBJECT

public:
    explicit CasePanel(QWidget *parent = 0);
    ~CasePanel();
    void setText(QStringList text);
    QFont caseFont();
    int labWidth();

private:
    Ui::CasePanel *ui;
    QFont* font;
    QString geteElidedText(QFont font, QString str, int MaxWidth);
    void paintEvent(QPaintEvent *);
};

#endif // CASEPANEL_H
