#ifndef WIDGET_H
#define WIDGET_H

#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QToolButton>
#include <QWidget>

#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

#include <FlyCapture2.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    QImage img;
    FlyCapture2::Camera cam;
    unsigned int numCameras;
    const FlyCapture2::Mode k_fmt7Mode = FlyCapture2::MODE_0;
    const FlyCapture2::PixelFormat k_fmt7PixFmt = FlyCapture2::PIXEL_FORMAT_MONO8;
    void PrintError(FlyCapture2::Error error);
    int RunSingleCamera( FlyCapture2::PGRGuid guid );
    //QImage Mat2QImage(const cv::Mat3b &src);
    QImage Mat2QImage(const cv::Mat_<double> &src);
    QImage Mat2QImage2(const cv::Mat &src);
};

#endif // WIDGET_H
