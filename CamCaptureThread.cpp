#include "CamCaptureThread.h"

using namespace cv;
using namespace std;
using namespace FlyCapture2;

CamCaptureThread::CamCaptureThread() :
    output(new QImage)
//    output(new QImage),
//    rawImage(new Image),
//    rgbImage(new Image)
{
    this->stopped = false;
    this->binaryOnOff = false;
}

void CamCaptureThread::stop()
{
    stopped = true;
    delete output;

}

void CamCaptureThread::run()
{
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
        // 2.1 get camera info
        // 2.2 setup camera
        Format7Info fmt7Info;
        bool supported;
        fmt7Info.mode = k_fmt7Mode;
        error = cam.GetFormat7Info(&fmt7Info, &supported);
        if (error != PGRERROR_OK) {
            error.PrintErrorTrace();
        }
        // 2.3 validate Format7ImageSettings
        Format7ImageSettings fmt7ImageSettings;
        fmt7ImageSettings.mode = k_fmt7Mode;
        fmt7ImageSettings.offsetX = 0;
        fmt7ImageSettings.offsetY = 0;
        fmt7ImageSettings.width = fmt7Info.maxWidth;
        fmt7ImageSettings.height = fmt7Info.maxHeight;
        fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;

        bool valid;
        Format7PacketInfo fmt7PacketInfo;
        error = cam.ValidateFormat7Settings(&fmt7ImageSettings, &valid, &fmt7PacketInfo);
        if (error != PGRERROR_OK) {
            error.PrintErrorTrace();
        }
        if (!valid) {
            cout << "Format7 settings are not valid" << endl;
        }

        error = cam.SetFormat7Configuration(&fmt7ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket );
        if (error != PGRERROR_OK) {
            error.PrintErrorTrace();
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
//            error = cam.RetrieveBuffer(rawImage);
            error = cam.RetrieveBuffer(&rawImage);
            if(error != PGRERROR_OK)
            {
                error.PrintErrorTrace();
                stopped = true;
            }
//            rawImage->Convert(PIXEL_FORMAT_BGR, rgbImage);
            rawImage.Convert(PIXEL_FORMAT_BGR, &rgbImage);

            unsigned int rowBytes =
                    (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
//                    (double)rgbImage->GetReceivedDataSize()/(double)rgbImage->GetRows();
            cv::Mat image =
                    cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),rowBytes);
//                    cv::Mat(rgbImage->GetRows(), rgbImage->GetCols(), CV_8UC3, rgbImage->GetData(),rowBytes);

            if(binaryOnOff)
            {
                cv::threshold(image, image, binaryValue, (100+150), CV_THRESH_BINARY);
            }

            *output = Mat2QImage(image);
            emit refresh(output);
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
//        delete rawImage;
//        delete rgbImage;
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

void CamCaptureThread::setBinaryOnOff(bool onOff)
{
    binaryOnOff = onOff;
}

void CamCaptureThread::setBinaryValue(unsigned int value)
{
    binaryValue = value;
}
