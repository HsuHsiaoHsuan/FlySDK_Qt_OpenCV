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
}

void Widget::on_pushButton_close_cam_clicked()
{
    thread->stop();
}
