#ifndef QIMAGELABEL_H
#define QIMAGELABEL_H

#include <QEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>

class QImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QImageLabel(QWidget *parent = 0);

protected:
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void leaveEvent(QEvent *);

    int x, y;

signals:
    void signal_MousePosition(int x, int y);
    void signal_MousePress(bool leftOrRight, QPoint);
    void signal_MouseRelease();
    void signal_MouseLeave();

public slots:

};

#endif // QIMAGELABEL_H
