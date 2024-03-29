#-------------------------------------------------
#
# Project created by QtCreator 2015-08-30T03:23:07
#
#-------------------------------------------------

QT      += core gui
QT      += concurrent widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TurBoCapture3000
TEMPLATE = app

INCLUDEPATH += /usr/include/opencv4

LIBS += -L/usr/local/lib \
    -lQt5Concurrent \
    -lgphoto2 \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_ml \
    -lopencv_video \
    -lopencv_features2d \
    -lopencv_calib3d \
    -lopencv_objdetect \
    -lopencv_videoio \
    -lopencv_flann \
    -lopencv_imgcodecs \
    -lopencv_highgui \
    -lopencv_xfeatures2d \
    -lopencv_stitching

SOURCES += main.cpp\
        turbocapture.cpp\
        mat2qimage.cpp \
    photo.cpp \
    stich.cpp

HEADERS  += turbocapture.h\
        mat2qimage.h \
    photo.h \
    stich.h

FORMS    += turbocapture.ui
