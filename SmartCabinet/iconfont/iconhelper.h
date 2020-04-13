#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QObject>
#include <QFont>
#include <QFontDatabase>
#include <QMutex>
#include <QLabel>
#include <QIcon>
#include <QList>
#include <QPushButton>
#include <QApplication>

class IconHelper:public QObject
{
    Q_OBJECT
private:
    explicit IconHelper(QObject *parent = 0);
    QList<QFont> lstFont;
    QFont iconFont;
    static IconHelper* _instance;
    QFont loadFont(QString fontSrc);
    void initFonts();

public:
    static IconHelper* Instance()
    {
        static QMutex mutex;
        if(!_instance)
        {
            QMutexLocker locker(&mutex);
            if(!_instance)
            {
                _instance = new IconHelper;
            }
        }
        return _instance;
    }

    void SetIcon(QLabel* lab, QChar c, int size = 10);
    void SetIcon(QPushButton* btn, QChar c, int size = 10);
    QIcon GetIcon(int backGroundSize, QChar c, int fontSize, QString color="#ffffff", QString backGroundColor="#00000000");
};

#endif // ICONHELPER_H
