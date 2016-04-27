#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T09:53:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qMQC_v1
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    CamCaptureThread.cpp

HEADERS  += widget.h \
    CamCaptureThread.h

FORMS    += widget.ui

INCLUDEPATH += .
INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /usr/local/include/opencv2/calib3d
INCLUDEPATH += /usr/local/include/opencv2/core
INCLUDEPATH += /usr/local/include/opencv2/features2d
INCLUDEPATH += /usr/local/include/opencv2/flann
INCLUDEPATH += /usr/local/include/opencv2/highgui
INCLUDEPATH += /usr/local/include/opencv2/imgcodecs
INCLUDEPATH += /usr/local/include/opencv2/imgproc
INCLUDEPATH += /usr/local/include/opencv2/ml
INCLUDEPATH += /usr/local/include/opencv2/objdetect
INCLUDEPATH += /usr/local/include/opencv2/photo
INCLUDEPATH += /usr/local/include/opencv2/shape
INCLUDEPATH += /usr/local/include/opencv2/stitching
INCLUDEPATH += /usr/local/include/opencv2/superres
INCLUDEPATH += /usr/local/include/opencv2/video
INCLUDEPATH += /usr/local/include/opencv2/videoio
INCLUDEPATH += /usr/local/include/opencv2/videostab
INCLUDEPATH += /usr/include/flycapture
INCLUDEPATH += /usr/include/flycapture/C

LIBS += `pkg-config opencv --cflags --libs`
LIBS += "-lflycapture"
