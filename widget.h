#ifndef WIDGET_H
#define WIDGET_H

#include <QEvent>
#include <QImage>
#include <QMouseEvent>
#include <QPoint>
#include <QToolButton>
#include <QWidget>

#include <iostream>

#include "CamCaptureThread.h"
#include "QImageLabel.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void refresh_cam(QImage *img);

    void on_pushButton_open_cam_clicked();
    void on_pushButton_close_cam_clicked();
    void on_pushButton_close_app_clicked();
    void on_checkBox_binary_stateChanged();         // binary
    void on_horizontalSlider_binary_valueChanged();
    void on_checkBox_canny_stateChanged();          // canny
    void on_horizontalSlider_canny_valueChanged();
    void on_checkBox_contour_stateChanged();        // contour
    void on_checkBox_minAreaRect_stateChanged();    // minAreaRect

    void slot_mouseMovingPosition(int x, int y);
    void slot_mousePressed(bool isLeftBtn, QPoint point);
    void slot_mouseRelease();
    void slot_mouseLeave();

private:
    Ui::Widget *ui;
    CamCaptureThread *thread;

};

#endif // WIDGET_H
