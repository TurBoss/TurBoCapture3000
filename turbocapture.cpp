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

#include "mat2qimage.h"

#include <vector>
#include <iostream>

#include <stdio.h>
#include <fcntl.h>

#include <QtSerialPort/QSerialPort>

#include <QMessageBox>

#include <gphoto2/gphoto2-camera.h>

#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/stitching.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

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


    txLedScene = new QGraphicsScene(this);
    ui->TxLed->setScene(txLedScene);

    rxLedScene = new QGraphicsScene(this);
    ui->RxLed->setScene(txLedScene);

    QBrush txLedBrush(Qt::green);
    QBrush txLedOffBrush(Qt::white);
    QPen txLedPen(Qt::black);

    QBrush rxLedBrush(Qt::green);
    QBrush rxLedOffBrush(Qt::white);
    QPen rxLedPen(Qt::black);

    /*txLedItem = txLedScene->addRect(0, 0, 15, 15, txLedPen, txLedBrush);
    rxLedItem = rxLedScene->addRect(0, 0, 15, 15, rxLedPen, rxLedBrush);

    txLedScene->clear();
    rxLedScene->clear();

    txLedItem = txLedScene->addRect(0, 0, 15, 15, txLedPen, txLedOffBrush);
    rxLedItem = rxLedScene->addRect(0, 0, 15, 15, rxLedPen, rxLedOffBrush);*/

    ledOn("TX");
    ledOn("RX");

    ledOff("TX");
    ledOff("RX");


    //Init Serial Port

    //-- Open the serial port
    serial = new QSerialPort(this);

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    openSerialPort();

    //Init Camera

    gp_camera_new (&camera);
    context = gp_context_new();

    gp_context_set_error_func (context, ctx_error_func, NULL);
    gp_context_set_status_func (context, ctx_status_func, NULL);

    ret = gp_camera_init(camera, context);

    if (ret < GP_OK) {
        QMessageBox msgBox;
        msgBox.setText("No camera auto detected.");
        msgBox.exec();
        printf("No camera auto detected.\n");
        gp_camera_free(camera);
    }
    else
        ui->camStatusLabel->setText("Online");
}

TurBoCapture::~TurBoCapture()
{

    delete ui;

    closeSerialPort();

    if (ret == GP_OK) {
        // close camera
        gp_camera_unref(camera);
        gp_context_unref(context);
    }
}

void TurBoCapture::ctx_error_func (GPContext *context, const char *str, void *data)
{
        fprintf  (stderr, "\n*** Contexterror ***\n%s\n",str);
        fflush   (stderr);
}

void TurBoCapture::ctx_status_func (GPContext *context, const char *str, void *data)
{
        fprintf  (stderr, "%s\n", str);
        fflush   (stderr);
}

void TurBoCapture::capture (const char *filename) {

    int fd, retval;
    CameraFile *file;
    CameraFilePath camera_file_path;

    // this was done in the libphoto2 example code, but doesn't seem to be necessary
    // NOP: This gets overridden in the library to /capt0000.jpg
    //snprintf(camera_file_path.folder, 1024, "/");
    //snprintf(camera_file_path.name, 128, "foo.jpg");

    // take a shot
    retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);

    if (retval) {
        // do some error handling, probably return from this function
    }

    printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

    fd = open(filename, O_CREAT | O_WRONLY, 0644);
    // create new CameraFile object from a file descriptor
    retval = gp_file_new_from_fd(&file, fd);

    if (retval) {
        // error handling
    }

    // copy picture from camera
    retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,
    GP_FILE_TYPE_NORMAL, file, context);

    if (retval) {
        // error handling
    }

    printf("Deleting\n");
    // remove picture from camera memory
    retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name,
    context);

    if (retval) {
        // error handling
    }

    // free CameraFile object
    gp_file_free(file);

    // Code from here waits for camera to complete everything.
    // Trying to take two successive captures without waiting
    // will result in the camera getting randomly stuck.
    int waittime = 2000;
    CameraEventType type;
    void *data;

    printf("Wait for events from camera\n");
    while(1) {
        retval = gp_camera_wait_for_event(camera, waittime, &type, &data, context);
        if(type == GP_EVENT_TIMEOUT) {
            break;
        }
        else if (type == GP_EVENT_CAPTURE_COMPLETE) {
            printf("Capture completed\n");
            waittime = 100;
        }
        else if (type != GP_EVENT_UNKNOWN) {
            printf("Unexpected event received from camera: %d\n", (int)type);
        }
    }
}

void TurBoCapture::stich(const char *fileA, const char *fileB)
{
    // Load the images
    Mat img_1 = imread( fileB );
    Mat img_2 = imread( fileA );

    Mat gray_image1;
    Mat gray_image2;

    // Convert to Grayscale
    cvtColor( img_1, gray_image1, CV_RGB2GRAY );
    cvtColor( img_2, gray_image2, CV_RGB2GRAY );

    if( !gray_image1.data || !gray_image2.data )
        { std::cout<< " --(!) Error reading images " << std::endl; }

    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    Ptr<SURF> detector = SURF::create();
    detector->setHessianThreshold(minHessian);

    std::vector<KeyPoint> keypoints_object, keypoints_scene;
    Mat descriptors_object, descriptors_scene;

    detector->detectAndCompute( img_1, Mat(), keypoints_object, descriptors_object );
    detector->detectAndCompute( img_2, Mat(), keypoints_scene, descriptors_scene );

    //-- Step 2: Calculate descriptors (feature vectors)
    Ptr<SURF> extractor = SURF::create();


    extractor->compute( gray_image1, keypoints_object, descriptors_object );
    extractor->compute( gray_image2, keypoints_scene, descriptors_scene );

    //-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );

    double max_dist = 0; double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_object.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );

    //-- Use only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< DMatch > good_matches;

    for( int i = 0; i < descriptors_object.rows; i++ )
    {
        if( matches[i].distance < 3*min_dist )
        {
            good_matches.push_back( matches[i]);
        }
    }

    std::vector< Point2f > obj;
    std::vector< Point2f > scene;

    for( uint i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
        scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }

    // Find the Homography Matrix
    Mat H = findHomography( obj, scene, CV_RANSAC );

    // Use the Homography Matrix to warp the images
    cv::Mat result;
    warpPerspective(img_1,result,H,cv::Size(img_1.cols+img_2.cols,img_1.rows));
    cv::Mat half(result,cv::Rect(0,0,img_2.cols,img_2.rows));
    img_2.copyTo(half);

    imwrite("pictures/result.tiff", result);
}

void TurBoCapture::stichv2(const char *fileA, const char *fileB)
{
    vector< Mat > vImg;
    Mat rImg;

    vImg.push_back( imread(fileA) );
    vImg.push_back( imread(fileB) );


    Stitcher stitcher = Stitcher::createDefault();


    unsigned long AAtime=0, BBtime=0; //check processing time
    AAtime = getTickCount(); //check processing time

    Stitcher::Status status = stitcher.stitch(vImg, rImg);

    BBtime = getTickCount(); //check processing time
    printf("%.2lf sec \n",  (BBtime - AAtime)/getTickFrequency() ); //check processing time

    if (Stitcher::OK == status){
        imwrite("pictures/result.tiff",rImg);
        printf("Done\n");
        QMessageBox msgBox;
        msgBox.setText("Compilation finished.");
        msgBox.exec();
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("Compilation failed.");
        msgBox.exec();
        printf("Stitching fail.\n");
    }
}

void TurBoCapture::stichv2_video(const char *file)
{
    vector< Mat > vImg;
    Mat rImg;
    Mat frame;

    VideoCapture cap(file); // open the default camera
    if(cap.isOpened()){  // check if we succeeded
        int i = 0;
        while (i < 2) {
            cap >> frame;

            vImg.push_back( frame );
            printf("Add Frame %i\n", i);
            i++;
        }


    }
    Stitcher stitcher = Stitcher::createDefault();


    unsigned long AAtime=0, BBtime=0; //check processing time
    AAtime = getTickCount(); //check processing time

    Stitcher::Status status = stitcher.stitch(vImg, rImg);

    BBtime = getTickCount(); //check processing time
    printf("%.2lf sec \n",  (BBtime - AAtime)/getTickFrequency() ); //check processing time

    if (Stitcher::OK == status){
        imwrite("pictures/result.tiff",rImg);
        printf("Done\n");
        QMessageBox msgBox;
        msgBox.setText("Compilation finished.");
        msgBox.exec();
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("Compilation failed.");
        msgBox.exec();
        printf("Stitching fail.\n");
    }
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

void TurBoCapture::ledOn(const char *led){

    QBrush ledBrush(Qt::green);
    QBrush ledOffBrush(Qt::white);
    QPen ledPen(Qt::black);

    if (strcmp( led, "TX") == 0){
        txLedScene->clear();
        txLedItem = txLedScene->addRect(0, 0, 15, 15, ledPen, ledBrush);
    }
    else if  (strcmp( led, "RX") == 0){
        rxLedScene->clear();
        rxLedItem = rxLedScene->addRect(0, 0, 15, 15, ledPen, ledBrush);
    }
}

void TurBoCapture::ledOff(const char *led){

    QBrush ledBrush(Qt::green);
    QBrush ledOffBrush(Qt::white);
    QPen ledPen(Qt::black);

    if (strcmp( led, "TX") == 0){
        txLedScene->clear();
        txLedItem = txLedScene->addRect(0, 0, 15, 15, ledPen, ledOffBrush);
    }
    else if  (strcmp( led, "RX") == 0){
        rxLedScene->clear();
        rxLedItem = rxLedScene->addRect(0, 0, 15, 15, ledPen, ledOffBrush);
    }

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
        ui->serialPortStatus->setText("/dev/ttyACM0");
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("No Serial Port.");
        msgBox.exec();
    }
}

void TurBoCapture::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
}

void TurBoCapture::writeData(const QByteArray &data)
{
    ledOn("TX");
    serial->write(data);
    ledOff("TX");
}

void TurBoCapture::readData()
{
    ledOn("RX");
    QByteArray data = serial->readAll();
    ledOff("RX");
}

void TurBoCapture::runMotor(){

    QByteArray msg("STEPFW");
    QByteArray stx("0x02");
    QByteArray etx("0x03");

    writeData(stx);

    writeData(msg);

    writeData(etx);
}

void TurBoCapture::on_horizontalSpinBox_valueChanged(double arg1)
{
    horizontal = arg1;

    h_line1->setLine(0,horizontal,width,horizontal);
    h_line2->setLine(0,height-horizontal,width,height-horizontal);
}

void TurBoCapture::on_verticalSpinBox_valueChanged(double arg1)
{
    vertical = arg1;

    v_line1->setLine(vertical,0,vertical,height);
    v_line2->setLine(width-vertical,0,width-vertical,height);
}

void TurBoCapture::on_takeFrameButton_clicked()
{


    if (ret == GP_OK) {

        // take 1 shot
        char filename[32];

        // do some capturing
        snprintf(filename, 32, "pictures/shot-%04d.nef", colsCount);
        printf("Capturing to file %s\n", filename);
        capture(filename);

        Mat imageCaptured = imread(filename);

        QImage qImage = Mat2QImage(imageCaptured);
        QPixmap pixmap = QPixmap::fromImage(qImage);

        scene->addPixmap(pixmap.scaled(QSize(width,height)));

        QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);
        QString qFilename = QString::fromUtf8(filename);
        addTreeChild(treeItem, qFilename);

        colsCount ++;

    }
    else{

        QMessageBox msgBox;
        msgBox.setText("No camera auto detected.");
        msgBox.exec();

    }

/*
    char filename[32];
    VideoCapture cap(-1); // open the default camera
    if(cap.isOpened()){  // check if we succeeded

        Mat frame;
        Mat viewFrame;
        Mat pic16bit;
        Mat roiFrame;

        cap >> frame; // get a new frame from camera

        double sizeX = width - (vertical*2);
        double sizeY = height - (horizontal*2);

        double sizeW = cap.get(CV_CAP_PROP_FRAME_WIDTH);    // Acquire input size
        double sizeH = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

        printf("View size X %4.2f\n",sizeX);
        printf("View size Y %4.2f\n",sizeY);

        printf("Image size W %4.2f\n",sizeW);
        printf("Image size H %4.2f\n",sizeH);


        //roiFrame = frame(Rect ());

        QImage qImage = Mat2QImage(frame);
        QPixmap pixmap = QPixmap::fromImage(qImage);

        scene->addPixmap(pixmap.scaled(QSize(width,height)));

        frame.convertTo(pic16bit, CV_16UC3, 255);


        snprintf(filename, sizeof(char) * 32, "pictures/file%i-%i.tiff", rowsCount,colsCount);

        QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);
        QString qFilename = QString::fromUtf8(filename);

        addTreeChild(treeItem, qFilename);

        imwrite(filename, pic16bit);

        colsCount ++;

    }
*/
}

void TurBoCapture::on_quitButton_clicked()
{
    qApp->quit();
}

void TurBoCapture::on_compileButton_clicked()
{
    printf("Compile\n");

    stichv2("pictures/1.tiff", "pictures/2.tiff");
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
}

void TurBoCapture::on_captureButton_clicked()
{
    //Start moving camera
    ui->captureButton->setEnabled(false);
    running = true;
    runMotor();
    //QFuture<void> motorRunning = run(this, &TurBoCapture::runMotor);
}
