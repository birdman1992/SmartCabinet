#ifndef MSGTIPS_H
#define MSGTIPS_H

#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QFont>
#include "manager/singleton.h"

class MsgTips : public QLabel, public Singleton<MsgTips>
{
    Q_OBJECT
    friend class Singleton<MsgTips>;
public:
    MsgTips(QWidget* parent = 0);
    ~MsgTips();

    void showText(QWidget *anchor, QStringList texts);
private:
    QFont* font;
    QRect calTextRect(QWidget *anchor, QStringList texts);
};

#endif // MSGTIPS_H
