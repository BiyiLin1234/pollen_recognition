#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

//用于多线程
class MyThread : public QThread
{
public:
    MyThread();
    void closeThread();
    void reset();
protected:
    virtual void run();
private:
    volatile bool isStop;//易失性变量
};

#endif // MYTHREAD_H
