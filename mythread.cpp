#include "mythread.h"
#include <QDebug>
#include <QMutex>

MyThread::MyThread(){
}
void MyThread::closeThread(){
    qDebug() << "已将信号位设置位关闭，待run函数的本次循环结束后，就可以";
    isStop = true;
}
void MyThread::run(){
    while(1){
        if(isStop){
            qDebug() << "now the thread is running id:" << QThread::currentThreadId();
        }
        sleep(2);
    }
}

void MyThread::reset(){
    isStop = true;
}
