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
    void setBinaryOnOff(bool onOff);          // binary
    void setBinaryValue(unsigned int value);
    void setCannyOnOff(bool onOff);
    void setCannyValue(unsigned int value);   // canny
    void setContourOnOff(bool onOff);         // contour
    void setMinAreaRectOnOff(bool onOff);     // min area rect
protected:
    void run();
private:
    // for camera setting
    unsigned int numCameras;
    const int k_numImages = 10;
    FlyCapture2::BusManager busMgr;
    FlyCapture2::Camera cam;
    FlyCapture2::Error error;
    const FlyCapture2::Mode k_fmt7Mode = FlyCapture2::MODE_1;
    FlyCapture2::PGRGuid guid;
    const FlyCapture2::PixelFormat k_fmt7PixFmt = FlyCapture2::PIXEL_FORMAT_MONO8;
//    FlyCapture2::Image *rawImage;
//    FlyCapture2::Image *rgbImage;

    // for image
    bool stopped;
    QImage Mat2QImage(const cv::Mat &src);
    QImage *output;
    bool binaryOnOff;
    unsigned int binaryValue;
    bool cannyOnOff;
    unsigned int cannyValue;
    bool contourOnOff;
    bool minAreaRectOnOff;
signals:
    void refresh(QImage *img);
};

#endif // CAMCAPTURETHREAD_H
