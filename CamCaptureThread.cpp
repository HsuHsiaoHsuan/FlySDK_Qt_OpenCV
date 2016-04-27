#include "CamCaptureThread.h"

using namespace std;
using namespace FlyCapture2;

CamCaptureThread::CamCaptureThread()
{
    this->stopped = false;
}

void CamCaptureThread::stop()
{
    stopped = true;
}

void CamCaptureThread::run()
{
    cout << "CamCaptureThread::run" << endl;

    error = busMgr.GetNumOfCameras(&numCameras);
    if(error != PGRERROR_OK)
    {
        error.PrintErrorTrace();
        stopped = true;
    }

    if(numCameras > 0)
    {
        // 1. get camera
        error = busMgr.GetCameraFromIndex(0, &guid);
        if(error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
            stopped = true;
        }
        // 2. connect to camera
        error = cam.Connect(&guid);
        if(error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
            stopped = true;
        }
        // 3. start capture
        error = cam.StartCapture();
        if(error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
            stopped = true;
        }
        Image rawImage;
        Image rgbImage;
        while(!stopped)
        {
            error = cam.RetrieveBuffer(&rawImage);
            if(error != PGRERROR_OK)
            {
                error.PrintErrorTrace();
                stopped = true;
            }
            rawImage.Convert( PIXEL_FORMAT_BGR, &rgbImage );

            unsigned int rowBytes =
                    (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
            cv::Mat image =
                    cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),rowBytes);

            output = Mat2QImage(image);
            emit refresh(&output);
        }
        // 4. stop camera
        error = cam.StopCapture();
        if(error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
        }
        // 5. disconnect camera
        error = cam.Disconnect();
        if(error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
        }
    }
}

QImage CamCaptureThread::Mat2QImage(const cv::Mat &src)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(src.type() == CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++)
            colorTable.push_back(qRgb(i, i, i));
            // Copy input Mat
            const uchar *qImageBuffer = (const uchar*)src.data;
            // Create QImage with same dimensions as input Mat
            QImage img(qImageBuffer, src.cols, src.rows, src.step, QImage::Format_Indexed8);
            img.setColorTable(colorTable);
            return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(src.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)src.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, src.cols, src.rows, src.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}