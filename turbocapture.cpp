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

#include "turbocapture.h"
#include "ui_turbocapture.h"

#include "photo.h"
#include "stich.h"

#include "mat2qimage.h"

#include <vector>
#include <iostream>

#include <stdio.h>
#include <fcntl.h>

#include <QtSerialPort/QSerialPort>

#include <QMessageBox>

using namespace std;

TurBoCapture::TurBoCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TurBoCapture)
{
    ui->setupUi(this);

    //Init GUI

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    QPen outlinePen(Qt::red);
    outlinePen.setWidth(1);

    width = ui->graphicsView->width()-5;
    height = ui->graphicsView->height()-5;

    h_line1 = scene->addLine(0,0,width,0,outlinePen); // Horizontal 1
    h_line2 = scene->addLine(0,height,width,height,outlinePen); // Horizontal 2

    v_line1 = scene->addLine(0,0,0,height,outlinePen); // Vertical 1
    v_line2 = scene->addLine(width,0,width,height,outlinePen); // Vertical 2

    QList<QGraphicsItem *> List;

    List.append(h_line1);
    List.append(h_line2);
    List.append(v_line1);
    List.append(v_line2);

    QGraphicsItemGroup *lines = scene->createItemGroup(List);

    lines->setZValue(999);

    colsCount = 0;
    rowsCount = 0;

    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList() << "Colum" << "Name");

    //Init Serial Port

    //-- Open the serial port
    serial = new QSerialPort(this);

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    openSerialPort();


    stepFWv = "STEPFWV\n";
    stepBWv = "STEPBWV\n";

    stepFWh = "STEPFWH\n";
    stepBWh = "STEPBWH\n";

    stx = "0x02\n";
    etx = "0x03\n";

    mSpeed = 50;
    ui->speedSpinBox->setValue(mSpeed);

    mStepps = 1;
    ui->steppsSpinBox->setValue(mStepps);

    frameSizeH = 56;
    frameSizeV = 48;

    ui->horizontalSurfaceSpinBox->setValue(frameSizeH);
    ui->verticalSurfaceSpinBox->setValue(frameSizeV);



    // create a timer
    motorSpeedTimer = new QTimer(this);

    // setup signal and slot
    connect(motorSpeedTimer, SIGNAL(timeout()), this, SLOT(runMotor()));

}

TurBoCapture::~TurBoCapture()
{
    delete ui;
    stopMotor();
    closeSerialPort();
}


void TurBoCapture::addTreeRoot(QString name)
{
    pic_cols = new QTreeWidgetItem(ui->treeWidget);
    pic_cols->setText(0, name);
}

void TurBoCapture::addTreeChild(QTreeWidgetItem *parent, QString name)
{
    pic_rows = new QTreeWidgetItem(pic_cols);
    parent->setText(1,name);
    parent->addChild(pic_rows);
}

void TurBoCapture::openSerialPort()
{
    serial->setPortName("/dev/ttyACM0");

    if(serial->open(QIODevice::ReadWrite)){
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        ui->serialPortStatus->setText(serial->portName());
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("No Serial Port.");
        msgBox.exec();
        ui->captureButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
    }
}

void TurBoCapture::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
}

void TurBoCapture::writeData(const QByteArray &data)
{
    serial->write(data);
    serial->waitForBytesWritten(1000);
}

void TurBoCapture::readData()
{
    QByteArray data = serial->readAll();
    serial->waitForReadyRead(1000);
}

void TurBoCapture::startMotor()
{
    writeData(stx);;
}
void TurBoCapture::runMotor()
{

    if (steppsH > 0){
        steppsH --;
        writeData(stepFWv);

        fprintf  (stdout, "%d\n",steppsH);
        fflush   (stdout);
    }
    else {
        motorSpeedTimer->stop();
        stopMotor();

        if (curPicsH > 0){
            takePicture();
            curPicsH --;
            steppsH = steppsPicH;

            if (curPicsH != 1){
                startMotor();
                motorSpeedTimer->start(mSpeed);
            }

            fprintf  (stdout, "%d\n",curPicsH);
            fflush   (stdout);
        }
        else{
            motorSpeedTimer->stop();
            stopMotor();
            ui->captureButton->setEnabled(true);
        }
    }
}

void TurBoCapture::stopMotor()
{

    writeData(etx);
}

void TurBoCapture::takePicture()
{

    fprintf  (stdout, "PIC TAKEN\n");
    fflush   (stdout);
}

void TurBoCapture::on_horizontalSpinBox_valueChanged(int h)
{
    horizontal = h;

    h_line1->setLine(0,horizontal,width,horizontal);
    h_line2->setLine(0,height-horizontal,width,height-horizontal);
}

void TurBoCapture::on_verticalSpinBox_valueChanged(int v)
{
    vertical = v;

    v_line1->setLine(vertical,0,vertical,height);
    v_line2->setLine(width-vertical,0,width-vertical,height);
}

void TurBoCapture::on_takeFrameButton_clicked()
{

}

void TurBoCapture::on_quitButton_clicked()
{
    qApp->quit();
}

void TurBoCapture::on_compileButton_clicked()
{
    printf("Compile\n");

    //stichv2("pictures/1.tiff", "pictures/2.tiff");
    //stichv2_video("pictures/01.mpeg");
}

void TurBoCapture::on_newRowButton_clicked()
{
    rowsCount ++;
    colsCount = 0;

    char rowsText[32];
    snprintf(rowsText, sizeof(char) * 32, "Col %i", rowsCount);
    addTreeRoot(rowsText);
}

void TurBoCapture::on_resetButton_clicked()
{
    //Reset camera position
    ui->captureButton->setEnabled(true);
    running = false;
    motorSpeedTimer->stop();
    stopMotor();
}

void TurBoCapture::on_captureButton_clicked()
{
    //Start moving camera
    ui->captureButton->setEnabled(false);

    picsH = calcPics(surfaceH,frameSizeH);
    picsV = calcPics(surfaceV,frameSizeV);

    steppsPicH = calcStepps(mStepps, frameSizeH);
    steppsPicV = calcStepps(mStepps, frameSizeV);


    fprintf  (stdout, "Num Pics Horizontal : %d\n",picsH);
    fflush   (stdout);
    fprintf  (stdout, "Num Pics Vertical : %d\n",picsV);
    fflush   (stdout);

    fprintf  (stdout, "Num Stepps Horizontal : %d\n",steppsPicH);
    fflush   (stdout);
    fprintf  (stdout, "Num Stepps Vertical : %d\n",steppsPicV);
    fflush   (stdout);

    curPicsH = picsH;
    curPicsV = picsV;

    steppsH = steppsPicH;
    steppsV = steppsPicV;

    running = true;

    startMotor();

    takePicture();

    motorSpeedTimer->start(mSpeed);

}

void TurBoCapture::on_speedSpinBox_valueChanged(int speed)
{
    mSpeed = speed;
    motorSpeedTimer->setInterval(mSpeed);
}

void TurBoCapture::on_horizontalSurfaceSpinBox_valueChanged(int mm)
{
    surfaceH = mm;
}

void TurBoCapture::on_verticalSurfaceSpinBox_valueChanged(int mm)
{
    surfaceV = mm;
}

void TurBoCapture::on_steppsSpinBox_valueChanged(int stepps)
{
    mStepps = stepps;
}

int TurBoCapture::calcPics( int mm, int picSize)
{
    int numPics = mm/picSize;
    return numPics;
}

int TurBoCapture::calcStepps( int stepps, int picSize)
{
    int numStepps = picSize*stepps;
    return numStepps;
}
