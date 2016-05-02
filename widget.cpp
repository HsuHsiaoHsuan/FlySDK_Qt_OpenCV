#include "widget.h"
#include "ui_widget.h"

using namespace FlyCapture2;
using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    thread = NULL;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::refresh_cam(QImage *img)
{
    QPixmap tmp = QPixmap::fromImage(*img);
    ui->label_image->setPixmap(tmp);
    ui->label_image->setFixedSize(tmp.size());
}

void Widget::on_pushButton_open_cam_clicked()
{
    thread = new CamCaptureThread;

    connect(thread, SIGNAL(refresh(QImage*)), this, SLOT(refresh_cam(QImage*)));
    connect(ui->label_image, SIGNAL(signal_MousePosition(int,int)), this, SLOT(slot_mouseMovingPosition(int,int)));
    connect(ui->label_image, SIGNAL(signal_MousePress(bool, QPoint)), this, SLOT(slot_mousePressed(bool, QPoint)));
    connect(ui->label_image, SIGNAL(signal_MouseRelease()), this, SLOT(slot_mouseRelease()));
    connect(ui->label_image, SIGNAL(signal_MouseLeave()), this, SLOT(slot_mouseLeave()));

    thread->start();
    ui->pushButton_open_cam->setEnabled(false);
    ui->pushButton_close_cam->setEnabled(true);
    ui->groupBox->setEnabled(true);
    ui->groupBox_2->setEnabled(true);
    thread->setBinaryOnOff(ui->checkBox_binary->isChecked());
    thread->setBinaryValue(ui->horizontalSlider_binary->value());
    thread->setCannyOnOff(ui->checkBox_canny->isChecked());
    thread->setCannyValue(ui->horizontalSlider_canny->value());
    thread->setContourOnOff(ui->checkBox_contour->isChecked());

    ui->label_image->setMouseTracking(true);
}

void Widget::on_pushButton_close_cam_clicked()
{
    thread->stop();
    ui->pushButton_open_cam->setEnabled(true);
    ui->pushButton_close_cam->setEnabled(false);
    ui->groupBox->setEnabled(false);

    ui->checkBox_minAreaRect->setChecked(false);
    ui->groupBox_2->setEnabled(false);
}

void Widget::on_pushButton_close_app_clicked()
{
    if(thread->isRunning())
    {
        thread->stop();
    }
    QApplication::quit();
}

void Widget::on_checkBox_binary_stateChanged()
{
    thread->setBinaryOnOff(ui->checkBox_binary->isChecked());
}

void Widget::on_horizontalSlider_binary_valueChanged()
{
    thread->setBinaryValue(ui->horizontalSlider_binary->value());
}

void Widget::on_checkBox_canny_stateChanged()
{
    thread->setCannyOnOff(ui->checkBox_canny->isChecked());
    if(ui->checkBox_canny->isChecked())
    {
        ui->checkBox_contour->setEnabled(true);
    }
    else
    {
        ui->checkBox_contour->setChecked(false);
        ui->checkBox_contour->setEnabled(false);
    }
}

void Widget::on_horizontalSlider_canny_valueChanged()
{
    thread->setCannyValue(ui->horizontalSlider_canny->value());
}

void Widget::on_checkBox_contour_stateChanged()
{
    thread->setContourOnOff(ui->checkBox_contour->isChecked());
    if(ui->checkBox_contour->isChecked())
    {
        ui->checkBox_minAreaRect->setEnabled(true);
    }
    else
    {
        ui->checkBox_minAreaRect->setChecked(false);
        ui->checkBox_minAreaRect->setEnabled(false);
    }
}

void Widget::on_checkBox_minAreaRect_stateChanged()
{
    thread->setMinAreaRectOnOff(ui->checkBox_minAreaRect->isChecked());
}

void Widget::slot_mouseMovingPosition(int x, int y)
{
//    cout << "x: " << x << ", y: " << y << endl;
    thread->setMovingPoint(x, y);
}

void Widget::slot_mousePressed(bool isLeftBtn, QPoint point)
{
    cout << "mouse pressed " << isLeftBtn << endl;
    if(isLeftBtn)
    {
        thread->setStartPoint(point.x(), point.y());
    }
    else
    {
        thread->cancelPoint();
    }
}

void Widget::slot_mouseRelease()
{
    cout << "mouse release" << endl;
}

void Widget::slot_mouseLeave()
{
    cout << "mouse leave" << endl;

}
