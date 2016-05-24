#include "CamCaptureThread.h"

using namespace cv;
using namespace std;
using namespace FlyCapture2;

CamCaptureThread::CamCaptureThread() :
    output(new QImage)
{
    this->stopped = false;
    this->binaryOnOff = false;
    this->cannyOnOff = false;
    this->minAreaRectOnOff = false;

    this->startDrawing = false;
    this->endPoint = Point(-1, -1);

    sampledImage = cv::Mat::zeros(3, 3, CV_8UC1);
}

void CamCaptureThread::stop()
{
    stopped = true;
    this->startDrawing = false;
    this->endPoint = Point(-1, -1);
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
        RNG& rng = theRNG(); // for draw contours

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
            Rect roi;
            error = cam.RetrieveBuffer(&rawImage);
            if(error != PGRERROR_OK)
            {
                error.PrintErrorTrace();
                stopped = true;
            }
            rawImage.Convert(PIXEL_FORMAT_BGR, &rgbImage);

            unsigned int rowBytes =
                    (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
            cv::Mat image =
                    cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),rowBytes);

            if(this->startDrawing)
            {
                cv::rectangle(image, startPoint, movingPoint, Scalar(255, 255, 0), 1);
            }
            else
            {
                if(endPoint.x != -1)
                {
                    int startX = (startPoint.x <= endPoint.x) ? startPoint.x : endPoint.x;
                    int startY = (startPoint.y <= endPoint.y) ? startPoint.y : endPoint.y;
                    int width = std::abs(endPoint.x - startPoint.x);
                    int height = std::abs(endPoint.y - startPoint.y);

                    if(width > 0 && height > 0)
                    {
//                        Rect roi = Rect(startX, startY, width, height);
                        roi = Rect(startX, startY, width, height);

                        cout << "startPoint.x " << startPoint.x << endl;
                        cout << "startPoint.y " << startPoint.y << endl;
                        cout << "endPoint.x " << endPoint.x << endl;
                        cout << "endPoint.y " << endPoint.y << endl;
                        sampledImage = image(roi).clone();

                        cv::rectangle(image, roi, Scalar(255, 255, 0), 1);
                    }

                }
            }

            if(binaryOnOff)
            {
                this->binaryConvert(image);
                if(sampledImage.type() > 0)
                {
                    this->binaryConvert(sampledImage);
                }
            }

            if(cannyOnOff)
            {
                this->cannyConvert(image);
                if(sampledImage.type() > 0)
                {
                    this->cannyConvert(sampledImage);
                }
            }

            if(contourOnOff)
            {
//                this->contourConvert(image);
                this->contourConvert(sampledImage);
                imshow("roi", sampledImage);
            }

            *output = Mat2QImage(image);
            imshow("roi", sampledImage);
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

void CamCaptureThread::binaryConvert(Mat &img)
{
    cv::threshold(img, img, binaryValue, (100+150), CV_THRESH_BINARY);
}

void CamCaptureThread::cannyConvert(Mat &img)
{
    cv::Mat tmp;
    img.copyTo(tmp);
    cv::Canny(tmp, img, cannyValue, 301, 3);
}

void CamCaptureThread::rotateImage(Mat &img)
{
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    cv::findContours(img, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    vector<cv::Point> biggestContour = contours[0];
    for (int x = 0; x < contours.size(); x++)
    {
        if (contours[x].size() > biggestContour.size())
        {
            biggestContour = contours[x];
        }
    }

    RotatedRect box = minAreaRect(biggestContour);
    Point2f vertex[4];
    box.points(vertex);

    Mat M, rotated, cropped;
    float box_angle = box.angle;
    Size box_size = box.size;
    if (box_angle < -45)
    {
        box_angle += 90.0;
        swap(box_size.width, box_size.height);
    }
    // get the rotation matrix
    M = getRotationMatrix2D(box.center, box_angle, 1.0);
    // perform the affine transformation
    warpAffine(img, rotated, M, img.size(), INTER_CUBIC);
    imshow("rotated", rotated);
    img = rotated;
}

void CamCaptureThread::contourConvert(Mat &img)
{
    //this->rotateImage(img);
//    vector<cv::Point> bigContour;
//    vector<cv::Point> smallContour;

    RNG& rng = theRNG();

    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    cv::findContours(img, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
    img = cv::Mat::zeros(img.size(), CV_8UC3);

//    if (contours[0].size() < contours[1].size())
//    {
//        smallContour = contours[0];
//        bigContour = contours[1];
//    }

    for (int x = 0; x < contours.size(); x++) {
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); // random color
        drawContours(img, contours, x, color, 2, 8, hierarchy, 0, Point());
    }

    if(minAreaRectOnOff) // find the minAreaRect
    {
        RotatedRect bigOne;
        RotatedRect smallOne;

        if (contours[0].size() < contours[1].size())
        {
            smallOne = minAreaRect(contours[1]);
            bigOne = minAreaRect(contours[0]);
        }
        else
        {
            smallOne = minAreaRect(contours[0]);
            bigOne = minAreaRect(contours[1]);
        }

        Point2f bigVertex[4];
        Point2f smallVertex[4];

        smallOne.points(bigVertex);
        bigOne.points(smallVertex);

        cout << bigVertex[0] << endl;
        cout << bigVertex[1] << endl;
        cout << bigVertex[2] << endl;
        cout << bigVertex[3] << endl;

        cv::line(img, Point2f(bigVertex[1].x, smallVertex[1].y), bigVertex[1], Scalar(255, 0, 0), 2, LINE_AA);


//        vector<cv::Rect> boundRect(contours.size());
        for (int x = 0; x < contours.size(); x++) {
            RotatedRect box = minAreaRect(contours[x]);
            Point2f vertex[4];
            box.points(vertex);
//            cout << "Points :: angle:: " << box.angle << " size:: " << box.size << " size.area:: " << box.size.area() << endl;

            for (int y = 0; y < 4; y++) {
                //cv::line(img, vertex[y], vertex[(y + 1) % 4], Scalar(0, 255, 0), 2, LINE_AA);
//                std::ostringstream tmp;
//                tmp <<  vertex[y];
//                cout << tmp.str() << endl;
//                putText(img, tmp.str(), vertex[y], CV_FONT_HERSHEY_COMPLEX, 0.3, Scalar(0, 0, 255));
            }
            cout << "-----------------" << endl;
/*
            double dist_a = cv::norm(vertex[0] - vertex[1]);
            double dist_b = cv::norm(vertex[1] - vertex[2]);
            std::ostringstream width;
            std::ostringstream height;
            if (dist_a > dist_b) {
                height << dist_a;
                width << dist_b;
            } else {
                height << dist_b;
                width << dist_a;
            }

            putText(img, "H:" + height.str(), Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 0.3, Scalar(0, 0, 255));
            putText(img, "W:" + width.str(), Point(50, 80), CV_FONT_HERSHEY_COMPLEX, 0.3, Scalar(0, 0, 255));
*/
        }
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

void CamCaptureThread::setCannyOnOff(bool onOff)
{
    cannyOnOff = onOff;
}

void CamCaptureThread::setCannyValue(unsigned int value)
{
    cannyValue = value;
}

void CamCaptureThread::setContourOnOff(bool onOff)
{
    contourOnOff = onOff;
}

void CamCaptureThread::setMinAreaRectOnOff(bool onOff)
{
    minAreaRectOnOff = onOff;
}

void CamCaptureThread::setMovingPoint(int x, int y)
{
    movingPoint = cv::Point(x, y);
}

void CamCaptureThread::setStartPoint(int x, int y)
{
    if(this->startDrawing) // if it's already true
    {
        this->setEndPoint(x, y);
    }
    else
    {
        this->startDrawing = true;
        startPoint = cv::Point(x, y);
        endPoint = cv::Point(-1, -1);
    }
}

void CamCaptureThread::setEndPoint(int x, int y)
{
    startDrawing = false;
    endPoint = cv::Point(x, y);
    //pointList.push_back(endPoint);
}

void CamCaptureThread::cancelPoint()
{
    startDrawing = false;
    endPoint = cv::Point(-1, -1);
    cv::destroyWindow("roi");
    sampledImage = cv::Mat::zeros(3, 3, CV_8UC1);
}
