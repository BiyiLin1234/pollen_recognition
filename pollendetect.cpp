//头文件区
#include "pollendetect.h"
#include "ui_pollendetect.h"
#include <QMovie>
#include <QSplashScreen>
#include <vector>
#include <QDebug>
#include <QMessageBox>
#include <QSpinBox>
#include <QFileDialog>
#include <QThread>
#include "mythread.h"

using namespace std;

//ui界面的构造函数
PollenDetect::PollenDetect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PollenDetect)
{
    ui->setupUi(this);
    init();
}

PollenDetect::~PollenDetect()
{
    delete ui;
}
//---------------------------------------------------
//初始化
void PollenDetect::init()
{
    //设置检测最大最小半径
    _minRadius = 100;
    _maxRadius = 900;
    //二值化的阈值默认140
    _valve = 140;
    ui->spinBox->setValue(140);
    //设置二值化阈值调节的spinBox
    ui->spinBox->setRange(0,255);
    ui->verticalSlider_cannyMax->setRange(0,255);
    ui->verticalSlider_cannyMin->setRange(0,255);
    thread_1 = new MyThread;
    connect(thread_1,&MyThread::finished,this,[=]{
        qDebug() << "thread has been finished\n";
    });
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(btn_clickable()));
    m_timer.start(20);
}
//------------------------------------------------------

//------------------------------------------------------
//功能函数
//生成随机color
cv::Vec3b RandomColor(int value)
{
    value=value%255;  //生成0~255的随机数
    cv::RNG rng;
    int aa=rng.uniform(0,value);
    int bb=rng.uniform(0,value);
    int cc=rng.uniform(0,value);
    return cv::Vec3b(aa,bb,cc);
}
//将cv::Mat转换为QImage，用于在Qt的UI界面上显示
bool PollenDetect::Mat2QImage(const cv::Mat &MatImg, cv::Mat& mid, QImage &qimg)
{
    mid = MatImg.clone();
    cv::resize(mid,mid,mid.size()/3);
    if(mid.channels() == 1){
        cv::cvtColor(mid,mid,CV_GRAY2BGR);
    }
    cv::cvtColor(mid,mid,CV_BGR2RGB);//Mat图片默认是BGR格式的，但是QImage是是RGB格式的，这里还必须用指针。但是随着图片的增多，内存问题。。。看来不能用函数解决了。
    qimg = QImage((const unsigned char*)((mid).data), (mid).cols, (mid).rows, QImage::Format_RGB888);
    return 1;
}
//---------------------------------------------------------

//----------------------------------------------------------
//界面和内部变量的数值交换
//移动spinBox的时候，将spinBox的值赋到_value中。
void PollenDetect::on_spinBox_valueChanged(int arg1)
{
    _valve = arg1;
}
void PollenDetect::on_verticalSlider_valueChanged(int value)
{
    _valve = value;
    on_btn_gray2bianry_clicked();
    ui->spinBox->setValue(value);
}
void PollenDetect::on_spinBox_min_valueChanged(int arg1)
{
    _minRadius = arg1;
}
void PollenDetect::on_spinBox_max_valueChanged(int arg1)
{
    _maxRadius = arg1;
}
void PollenDetect::on_verticalSlider_cannyMin_valueChanged(int value)
{
    canny_min = value;
    on_btn_canny_clicked();
}

void PollenDetect::on_verticalSlider_cannyMax_valueChanged(int value)
{
    canny_max = value;
    on_btn_canny_clicked();
}

//-------------------------------------------------------------------------------------
//按钮操作
//打开操作按钮
void PollenDetect::on_btn_open_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"select image","E:\\data\\pollen");
    //========================================================
    //检验
    if(path==QString("")){
        QMessageBox::information(this,"tip","not selected");
        return;
    }
    srcImg = cv::imread(path.toStdString());
    srcForSave = srcImg.clone();
    if(srcImg.empty()){
        QMessageBox::information(this,"tip","not an image");
        return;
    }
        cv::resize(srcImg,srcImg,cv::Size(srcImg.cols/2,srcImg.rows/2));
        srcImgCircled = srcImg.clone();
        //显示原图
        QImage img;
        Mat2QImage(srcImg,srcImgMid,img);
        ui->label_imgSrc->setPixmap(QPixmap::fromImage(img));
}

//src转到灰度图
void PollenDetect::on_btn_src2gray_clicked()
{
    if(srcImg.empty())
        return;
    cv::cvtColor(srcImg,grayImg,CV_BGR2GRAY);
    QImage img;
    Mat2QImage(grayImg,grayImgMid,img);
    ui->label_imgGray->setPixmap(QPixmap::fromImage(img));
}
void PollenDetect::on_btn_canny_clicked()
{
    if(grayImg.empty()){
        return;
    }
    qDebug() << canny_max << " " << canny_min << endl;
    cv::Canny(grayImg,cannyImg,canny_min,canny_max);
    QImage img;
    Mat2QImage(cannyImg,grayImgMid,img);
    ui->label_Canny->setPixmap(QPixmap::fromImage(img));
}
//灰度图到二值图
void PollenDetect::on_btn_gray2bianry_clicked()
{
    if(grayImg.empty())
        return;
    cv::threshold(grayImg,binaryImg,_valve,255,CV_THRESH_BINARY);//< value = (value > threshold) ? max_value : 0
    QImage img;
    Mat2QImage(binaryImg,binaryImgMid,img);
    ui->label_imgBinary->setPixmap(QPixmap::fromImage(img));
}


//找圆，并画到
void PollenDetect::on_btn_binary2circles_clicked()
{
    if(binaryImg.empty())
        return;
    std::vector<cv::Vec3f> circles;
    /*InputArray image, OutputArray circles,
                                   int method, double dp, double minDist,
                                   double param1 = 100, double param2 = 100,
                                   int minRadius = 0, int maxRadius = 0 );*/
    //dp是累加器和原图的比例，param1是Canny边缘检测的高阈值，低阈值自动设置位高阈值的一半。param2是累加平面是否是圆的判定阈值。
    cv::HoughCircles(binaryImg,circles,CV_HOUGH_GRADIENT,1,200,100,30,_minRadius,_maxRadius);
    if(circles.empty()){
        qDebug() << "no circles";
        QMessageBox::information(this,"tip","no circles");
        return;
    }
    qDebug() << "num of circles" << circles.size() << endl;//看看找到了多少圆

    QMessageBox::information(this,"num of circles",QString::number(circles.size()));
    //画圆
    srcImgCircled = srcImg.clone();
    for(size_t i=0;i<circles.size();++i){
        cv::Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
        int radius = circles[i][2];
        cv::circle(srcImgCircled,center,3,cv::Scalar(255,0,255),4);
        cv::circle(srcImgCircled,center,radius,cv::Scalar(0,255,0),8);
    }
    srcForSave_circled = srcForSave.clone();
    for(size_t i=0;i<circles.size();++i){
        cv::Point center(cvRound(circles[i][0]*2),cvRound(circles[i][1]*2));
        int radius = circles[i][2]*2;
        cv::circle(srcForSave_circled,center,3,cv::Scalar(255,0,255),4);
        cv::circle(srcForSave_circled,center,radius,cv::Scalar(0,255,0),8);
    }
    QImage img;
    Mat2QImage(srcImgCircled,srcImgCircledMid,img);
    ui->label_imgCircles->setPixmap(QPixmap::fromImage(img));
}
//--------------------------------------------------------------------
//显示源图片
void PollenDetect::on_btn_src_clicked()
{
    if(srcImg.empty()){
        return;
    }
    cv::imshow("srcimg",srcImg);
}
//显示灰度图片
void PollenDetect::on_btn_gray_clicked()
{
    if(grayImg.empty())
        return;
    cv::imshow("grayImg", grayImg);
}
//显示二值图
void PollenDetect::on_btn_binary_clicked()
{
    if(binaryImg.empty())
        return;
    cv::imshow("binaryImg",binaryImg);
}
//显示circledImg图
void PollenDetect::on_btn_Circles_clicked()
{
    if(srcImgCircled.empty()){
        return;
    }
    cv::imshow("circledImg",srcImgCircled);
}
//------------------------------------------------------------------------------

void PollenDetect::on_pushButton_clicked()
{
    if(srcImgCircled.empty()){
        QMessageBox::information(this,"error","not circled yet");
        return;
    }
    cv::imwrite("F://1.png",srcForSave_circled);
    QMessageBox::information(this,"saved","saved");
}

//-------------------------------------------------------------------------------

void PollenDetect::on_pushButton_2_clicked()
{
    qDebug() << "主线程id" << QThread::currentThreadId();
    thread_1->reset();
    thread_1->start();
}

void PollenDetect::on_pushButton_3_clicked()
{
    thread_1->closeThread();
}

void PollenDetect::btn_clickable()
{
    if(srcImg.empty()){
        ui->btn_src2gray->setDisabled(true);
    }
    else{
        ui->btn_src2gray->setEnabled(true);
    }

    if(grayImg.empty()){
        ui->btn_gray2bianry->setDisabled(true);
        ui->btn_canny->setDisabled(true);
    }
    else{
        ui->btn_gray2bianry->setEnabled(true);
        ui->btn_canny->setEnabled(true);
    }

    if(binaryImg.empty()){
        ui->btn_binary2circles->setDisabled(true);
    }
    else{
        ui->btn_binary2circles->setEnabled(true);
    }
}
