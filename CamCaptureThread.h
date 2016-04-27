#ifndef CAMCAPTURETHREAD_H
#define CAMCAPTURETHREAD_H

#include <QDebug>
#include <QImage>
#include <QThread>

#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

#include <FlyCapture2.h>

class CamCaptureThread : public QThread
{
    Q_OBJECT
public:
    CamCaptureThread();
    void stop();
protected:
    void run();
private:
    bool stopped;
    FlyCapture2::Error error;
    FlyCapture2::BusManager busMgr;
    unsigned int numCameras;
    FlyCapture2::PGRGuid guid;
    FlyCapture2::Camera cam;
    const int k_numImages = 10;
    QImage Mat2QImage(const cv::Mat &src);
    QImage output;
//    const FlyCapture2::Mode k_fmt7Mode = FlyCapture2::MODE_0;
//    const FlyCapture2::PixelFormat k_fmt7PixFmt = FlyCapture2::PIXEL_FORMAT_MONO8;
signals:
    void refresh(QImage *img);
};

#endif // CAMCAPTURETHREAD_H
