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

#include "stich.h"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

Stich::Stich()
{

}

Stich::~Stich()
{

}

void Stich::stich(const char *fileA, const char *fileB)
{
    // Load the images
    Mat img_1 = imread( fileB );
    Mat img_2 = imread( fileA );

    Mat gray_image1;
    Mat gray_image2;

    // Convert to Grayscale
    cvtColor( img_1, gray_image1, COLOR_BGR2BGRA );
    cvtColor( img_2, gray_image2, COLOR_BGR2BGRA );

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
    Mat H = findHomography( obj, scene, RANSAC );

    // Use the Homography Matrix to warp the images
    cv::Mat result;
    warpPerspective(img_1,result,H,cv::Size(img_1.cols+img_2.cols,img_1.rows));
    cv::Mat half(result,cv::Rect(0,0,img_2.cols,img_2.rows));
    img_2.copyTo(half);

    imwrite("pictures/result.tiff", result);
}

void Stich::stichv2(const char *fileA, const char *fileB)
{
    vector< Mat > vImg;
    Mat rImg;

    vImg.push_back( imread(fileA) );
    vImg.push_back( imread(fileB) );


    Ptr<Stitcher> stitcher = Stitcher::create();

    unsigned long AAtime=0, BBtime=0; //check processing time
    AAtime = getTickCount(); //check processing time

    Stitcher::Status status = stitcher->stitch(vImg, rImg);

    BBtime = getTickCount(); //check processing time
    printf("%.2lf sec \n",  (BBtime - AAtime)/getTickFrequency() ); //check processing time

    if (Stitcher::OK == status){
        imwrite("pictures/result.tiff",rImg);
        printf("Done\n");

    }
    else{
        printf("Stitching fail.\n");
    }
}

void Stich::stichv2_video(const char *file)
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
    Ptr<Stitcher> stitcher = Stitcher::create();


    unsigned long AAtime=0, BBtime=0; //check processing time
    AAtime = getTickCount(); //check processing time

    Stitcher::Status status = stitcher->stitch(vImg, rImg);

    BBtime = getTickCount(); //check processing time
    printf("%.2lf sec \n",  (BBtime - AAtime)/getTickFrequency() ); //check processing time

    if (Stitcher::OK == status){
        imwrite("pictures/result.tiff",rImg);
        printf("Done\n");
    }
    else{
        printf("Stitching fail.\n");
    }
}
