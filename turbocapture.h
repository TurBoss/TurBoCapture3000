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


#include <QMainWindow>

#include <QtSerialPort/QSerialPort>

#include <QTimer>

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

    void addTreeRoot(QString name);

    void addTreeChild(QTreeWidgetItem *parent, QString name);

    void closeSerialPort();

    void openSerialPort();

    void writeData(const QByteArray &data);

    void readData();

    void startMotor();

    void runMotor();

    void stopMotor();

    void takePicture();

    int calcPics(int mm, int picSize );

    int calcStepps(int stepps, int picSize);

    void on_quitButton_clicked();

    void on_horizontalSpinBox_valueChanged(int h);

    void on_verticalSpinBox_valueChanged(int v);

    void on_takeFrameButton_clicked();

    void on_compileButton_clicked();

    void on_newRowButton_clicked();

    void on_resetButton_clicked();

    void on_captureButton_clicked();

    void on_speedSpinBox_valueChanged(int speed);

    void on_horizontalSurfaceSpinBox_valueChanged(int mm);

    void on_verticalSurfaceSpinBox_valueChanged(int mm);

    void on_steppsSpinBox_valueChanged(int stepps);

private:
    Ui::TurBoCapture *ui;

    QSerialPort *serial;

    QTimer *motorSpeedTimer;

    QByteArray stepFWv;
    QByteArray stepBWv;

    QByteArray stepFWh;
    QByteArray stepBWh;

    QByteArray stx;
    QByteArray etx;


    bool running;

    int width;
    int height;

    double horizontal;
    double vertical;

    int colsCount;
    int rowsCount;

    int surfaceH;
    int surfaceV;

    int mSpeed;
    int mStepps;

    int frameSizeH;
    int frameSizeV;

    int picsH;
    int picsV;

    int curPicsH;
    int curPicsV;

    int steppsPicH;
    int steppsPicV;

    int steppsH;
    int steppsV;

    QGraphicsScene *scene;

    QGraphicsLineItem *h_line1;
    QGraphicsLineItem *h_line2;
    QGraphicsLineItem *v_line1;
    QGraphicsLineItem *v_line2;


    QTreeWidgetItem *pic_cols;
    QTreeWidgetItem *pic_rows;

};

#endif // TURBOCAPTURE_H
