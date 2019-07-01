/*
 * 主要目标
 * 1.有一个打开文件按钮，点击后，打开文件，然后选中一张图片，便会展示
 *
*/
#include <QApplication>
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <QWidget>
#include "pollendetect.h"
int main(int argc,char * argv[]){
    QApplication app(argc,argv);
    PollenDetect p;
    p.show();
    return app.exec();
}
