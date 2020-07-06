#ifndef SINGLETON_H
#define SINGLETON_H

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

/*****************************************
 * 单例模板类，作为基类可以让派生类变成单例
 * 使用示例：
 * 非界面类：
class DerivedSingle:public Singleton<DerivedSingle>
{
    friend class Singleton<DerivedSingle>;
public:
   DerivedSingle(const DerivedSingle&)=delete;
   DerivedSingle& operator =(const DerivedSingle&)= delete;
private:
   DerivedSingle()=default;
};

 *界面类：
class SingleTest : public QWidget,public Singleton<SingleTest>
{
    Q_OBJECT
    friend class Singleton<SingleTest>;
public:

private:
    explicit SingleTest(QWidget *parent = nullptr);

signals:

public slots:
};
 *
*****************************************/

template<typename T>
class Singleton{
public:
    static T& instance(){
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        static T _instance;
        return _instance;
    }
    virtual ~Singleton(){
        qDebug()<<"singelton delete.";
    }
    Singleton(const Singleton&)=delete;
    Singleton& operator =(const Singleton&)=delete;
protected:
    Singleton(){
    }
};

#endif // SINGLETON_H
