#ifndef GLOBALAPP_H
#define GLOBALAPP_H

#include <QApplication>
#include <QWidget>
#include "mainwidget.h"

class GlobalApp : public QApplication
{
public:
    GlobalApp(int &argc,char **argv);
    bool notify(QObject*, QEvent *);
    void setWidows(MainWidget*);

private:
    MainWidget* winToTell;
};

#endif // GLOBALAPP_H
