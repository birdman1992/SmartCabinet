#include "globalapp.h"

GlobalApp::GlobalApp(int &argc, char **argv):
    QApplication(argc,argv)
{

}

bool GlobalApp::notify(QObject *obj, QEvent *e)
{
    if(e->type() == QEvent::KeyPress)//按键事件
    {

    }
    else if(e->type() == QEvent::MouseButtonPress)//鼠标事件
    {
        winToTell->globalTouch();
    }
    return QApplication::notify(obj,e);
}

void GlobalApp::setWidows(MainWidget *w)
{
    winToTell = w;
}

