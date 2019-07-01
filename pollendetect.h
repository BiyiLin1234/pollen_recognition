#ifndef POLLENDETECT_H
#define POLLENDETECT_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QPixmap>
#include <QImage>
#include <QTimer>

class MyThread; //前置声明

namespace Ui {
class PollenDetect;
}

class PollenDetect : public QWidget
{
    Q_OBJECT
public:
    explicit PollenDetect(QWidget *parent = 0);
    ~PollenDetect();
    void ChangeImage(const cv::Mat&,cv::Mat&, int);
private slots:
    void on_spinBox_valueChanged(int arg1);

    void on_btn_open_clicked();

    void on_btn_src_clicked();

    void on_btn_gray_clicked();

    void on_btn_binary_clicked();

    void on_btn_Circles_clicked();

    void on_btn_src2gray_clicked();

    void on_btn_gray2bianry_clicked();

    void on_btn_binary2circles_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_spinBox_max_valueChanged(int arg1);

    void on_spinBox_min_valueChanged(int arg1);
    void on_btn_canny_clicked();

    void on_verticalSlider_cannyMin_valueChanged(int value);

    void on_verticalSlider_cannyMax_valueChanged(int value);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void btn_clickable();

private:

    Ui::PollenDetect *ui;
    cv::Mat srcImg;
    cv::Mat grayImg;
    cv::Mat binaryImg;
    cv::Mat srcImgCircled;
    //由于bgr/rgb不兼容创建的中间变量
    cv::Mat srcImgMid;
    cv::Mat grayImgMid;
    cv::Mat cannyImg;
    cv::Mat binaryImgMid;
    cv::Mat circlesImgMid;
    cv::Mat srcImgCircledMid;
    cv::Mat srcForSave;//没有压缩的
    cv::Mat srcForSave_circled;

    int _valve;//用来存储二值化的值
    int _minRadius;//检测的圆的最小半径
    int _maxRadius;//检测的圆的最大半径
    int canny_max;
    int canny_min;

    MyThread * thread_1;
    uchar thresh;
    QTimer m_timer;
public:
    bool Mat2QImage(const cv::Mat&, cv::Mat&, QImage&);//转换图片数据类型

    void init();
};
#endif // POLLENDETECT_H
