#include "QImageLabel.h"

QImageLabel::QImageLabel(QWidget *parent) :
    QLabel(parent)
{

}

void QImageLabel::mouseMoveEvent(QMouseEvent *ev)
{
    this->x = ev->x();
    this->y = ev->y();
    emit signal_MousePosition(ev->x(), ev->y());
}

void QImageLabel::mousePressEvent(QMouseEvent *ev)
{
    bool isLeftBtn = false;
    if (ev->buttons() == Qt::LeftButton)
    {
        isLeftBtn = true;
    }
    emit signal_MousePress(isLeftBtn, ev->pos());
}

void QImageLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    emit signal_MouseRelease();
}

//void QImageLabel::mouseReleaseEvent(QMouseEvent *ev)
//{
//    emit signal_MouseRelease();
//}

void QImageLabel::leaveEvent(QEvent *)
{
    emit signal_MouseLeave();
}
