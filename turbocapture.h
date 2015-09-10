/*
* Copyright (C) 2015 Jose Luis Toledano Lopez <j.l.toledano.l@gmail.com>
*
* This file is part of TurBoCapture3000.
*
* TurBoCapture3000 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* TurBoCapture3000 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with ImageQ. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TURBOCAPTURE_H
#define TURBOCAPTURE_H

#include <gphoto2/gphoto2-camera.h>

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>

#include <QTreeWidget>

#include <QDialog>

#include <QLabel>

#include <QString>

namespace Ui {

class TurBoCapture;
}

class TurBoCapture : public QMainWindow
{
    Q_OBJECT

public:
    explicit TurBoCapture(QWidget *parent = 0);
    ~TurBoCapture();

private slots:


    static void ctx_error_func (GPContext *context, const char *str, void *data);

    static void ctx_status_func (GPContext *context, const char *str, void *data);


    void addTreeRoot(QString name);

    void addTreeChild(QTreeWidgetItem *parent, QString name);

    void capture(const char *filename);

    void stich(const char *fileA, const char *fileB);

    void stichv2(const char *fileA, const char *fileB);

    void stichv2_video(const char *file);

    void ledOn(const char *led);

    void ledOff(const char *led);

    void closeSerialPort();

    void openSerialPort();

    void writeData(const QByteArray &data);

    void readData();

    void runMotor();

    void on_quitButton_clicked();

    void on_horizontalSpinBox_valueChanged(double arg1);

    void on_verticalSpinBox_valueChanged(double arg1);

    void on_takeFrameButton_clicked();

    void on_compileButton_clicked();

    void on_newRowButton_clicked();

    void on_resetButton_clicked();

    void on_captureButton_clicked();

private:
    Ui::TurBoCapture *ui;

    QSerialPort *serial;

    int ret;

    bool running;

    Camera *camera;
    GPContext *context;

    int width;
    int height;

    double horizontal;
    double vertical;

    int colsCount;
    int rowsCount;

    QGraphicsScene *scene;
    QGraphicsLineItem *h_line1;
    QGraphicsLineItem *h_line2;
    QGraphicsLineItem *v_line1;
    QGraphicsLineItem *v_line2;

    QGraphicsScene *txLedScene;
    QGraphicsRectItem *txLedItem;

    QGraphicsScene *rxLedScene;
    QGraphicsRectItem *rxLedItem;


    QTreeWidgetItem *pic_cols;
    QTreeWidgetItem *pic_rows;

};

#endif // TURBOCAPTURE_H
