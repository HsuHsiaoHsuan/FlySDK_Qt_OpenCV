#ifndef CAMCAPTURETHREAD_H
#define CAMCAPTURETHREAD_H

#include <QDebug>
#include <QImage>
#include <QThread>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <vector>

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

    void setMovingPoint(int x, int y);
    void setStartPoint(int x, int y);
    void setEndPoint(int x, int y);
    void cancelPoint();
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
    void binaryConvert(cv::Mat &img);      // binary
    bool binaryOnOff;
    unsigned int binaryValue;
    void cannyConvert(cv::Mat &img);       // canny
    bool cannyOnOff;
    unsigned int cannyValue;
    void rotateImage(cv::Mat &img);        // rotate image
    void contourConvert(cv::Mat &img);     // contour
    bool contourOnOff;
    bool minAreaRectOnOff;                 // minAreaRect
    std::vector<cv::Point> pointList;

    cv::Mat sampledImage;
    bool startDrawing;
    cv::Point movingPoint;
    cv::Point startPoint;
    cv::Point endPoint;
signals:
    void refresh(QImage *img);
};

#endif // CAMCAPTURETHREAD_H
