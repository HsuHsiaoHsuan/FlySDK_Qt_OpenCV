#include "widget.h"
#include "ui_widget.h"

using namespace FlyCapture2;
using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    FlyCapture2::Error error;
    FlyCapture2::BusManager busMgr;

    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK) {
        error.PrintErrorTrace();
        //return -1;
    }

    ui->label->setText(QString::number(numCameras));
    cout << "????? " << numCameras << endl;

    if (numCameras > 0)
    {
        PGRGuid guid;
        error = busMgr.GetCameraFromIndex(0, &guid);
        if (error != PGRERROR_OK)
        {
            PrintError(error);
        }
        //RunSingleCamera(guid);
    }

    cv::Mat image;
    image = cv::imread("/home/zhenhai/jobs/testQt/Lenna.png");
    imshow("OpenCV", image);

    QImage tmpImg = Mat2QImage2(image);
    ui->toolButton->setIconSize(QSize(tmpImg.width(), tmpImg.height()));
    ui->toolButton->setIcon(QPixmap::fromImage(tmpImg));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::PrintError( FlyCapture2::Error error ) {
    error.PrintErrorTrace();
}

QImage Widget::Mat2QImage2(const cv::Mat &src)
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

int Widget::RunSingleCamera( PGRGuid guid ) {
    int w = 1280;
    int h = 1024;

    const int k_numImages = 10;
    FlyCapture2::Error error;

    // Connect to a camera
    error = cam.Connect(&guid);
    if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
    }

    // Start capturing images
    error = cam.StartCapture();
    if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
    }

    Image rawImage;
    Image rgbImage;
    while(1)
    {
        error = cam.RetrieveBuffer(&rawImage);
        if(error != PGRERROR_OK)
        {
            PrintError(error);
            return -1;
        }
        // Convert to RGB
        rawImage.Convert( PIXEL_FORMAT_BGR, &rgbImage );

        unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();

        QImage img(rgbImage.GetData(), rgbImage.GetCols(), rgbImage.GetRows(), rowBytes, QImage::Format_RGB32);

//        QByteArray buffer(rowBytes * h, 0xFF);
//        uchar * p = (uchar*)buffer.data() + rowBytes;
//        QImage img(p, w, h, rowBytes, QImage::Format_ARGB32);
//        ui->toolButton->setIconSize(QSize(img.width(), img.height()));
//        ui->toolButton->setIcon(QPixmap::fromImage(img));
    }
}
