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
    ui->toolButton->setIconSize(QSize(img->width(), img->height()));
    ui->toolButton->setIcon(QPixmap::fromImage(*img));
}

void Widget::on_pushButton_open_cam_clicked()
{
    thread = new CamCaptureThread;
    connect(thread, SIGNAL(refresh(QImage*)), this, SLOT(refresh_cam(QImage*)));
    thread->start();
    ui->pushButton_open_cam->setEnabled(false);
    ui->pushButton_close_cam->setEnabled(true);
    ui->groupBox->setEnabled(true);
    thread->setBinaryOnOff(ui->checkBox_binary->isChecked());
    thread->setBinaryValue(ui->horizontalSlider_binary->value());
}

void Widget::on_pushButton_close_cam_clicked()
{
    thread->stop();
    ui->pushButton_open_cam->setEnabled(true);
    ui->pushButton_close_cam->setEnabled(false);
    ui->groupBox->setEnabled(false);
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
