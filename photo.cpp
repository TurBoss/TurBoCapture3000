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

#include "photo.h"

using namespace std;

Photo::Photo()
{

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
    else{
        //ui->camStatusLabel->setText("Online");
    }
}

Photo::~Photo()
{

    if (ret == GP_OK) {
        // close camera
        gp_camera_unref(camera);
        gp_context_unref(context);
    }
}

void Photo::ctx_error_func (GPContext *context, const char *str, void *data)
{
        fprintf  (stderr, "\n*** Contexterror ***\n%s\n",str);
        fflush   (stderr);
}

void Photo::ctx_status_func (GPContext *context, const char *str, void *data)
{
        fprintf  (stderr, "%s\n", str);
        fflush   (stderr);
}

void Photo::capture (const char *filename) {

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



/*void Photo::shootPhoto () {

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
//}
