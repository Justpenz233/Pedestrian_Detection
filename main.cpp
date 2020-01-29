//
//  main.cpp
//  Peopel_Detection
//
//  Created by 李斯祺 on 2020/1/26.
//  Copyright © 2020 李斯祺. All rights reserved.
//

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include "People.h"


#define CurrentPath "/Users/makik/CLionProjects/President_Detection/"

using namespace std;
using namespace cv;


double MinPeopelArea = 0;
double MaxPeopleArea = 10000;

list <Rect> DetectedContours;
list <People> DetectedPeople;

string MakePath(const string s){
    return CurrentPath + s;
}

void NextFram(VideoCapture &cap,double Sec){
    for(int i = 1;i <= Sec * 30;i ++){
        Mat fram;
        cap >> fram;
    }
}

int main() {



    namedWindow("Example2", WINDOW_AUTOSIZE);
    
    VideoCapture cap;
    cap.open(MakePath("demo.mp4"));
    
    auto fgmask = createBackgroundSubtractorMOG2(100);
    
    Mat frame;

    while (cap.isOpened()) {
        NextFram(cap, 0.05);//每0.1秒处理一次
        
        cap >> frame;
        
        if (frame.empty())
            break;
        
        Mat gray, blur;
        
        //转灰度图
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        
        //提取前景
        Mat MovingImg;
        fgmask->apply(gray, MovingImg);
        
        //消除噪点
        //medianBlur(MovingImg, blur, 5); //中值滤波
        GaussianBlur(MovingImg, blur, Size(25, 25), 1, 1); //高斯滤波
        //bilateralFilter(MovingImg, blur, 5, 2, 2); //双边滤波
        //cv::blur(MovingImg, blur, Size(5,5)); //均值滤波
        
        //形态学 腐蚀
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
        morphologyEx(blur, blur, MORPH_CLOSE, kernel);
        
        
        //高斯阴影消除
        threshold(blur, blur, 127, 255, THRESH_BINARY);
        
        //提取轮廓
        vector<vector<Point> > Contours;
        vector<Vec4i> hierarchy;
        findContours(blur, Contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        
        
        Mat GrayWithBorder;
        //inRange(gray, Scalar(0, 60, 32), Scalar(180, 255, 255), gray);
        //drawContours(frame, Contours, -1, Scalar(255 ,255, 255));

         for (const auto& t : Contours){
             double tArea = contourArea(t);
             if(tArea < MinPeopelArea and tArea > MaxPeopleArea)
                 continue;
             DetectedContours.push_back(boundingRect(t));

         }

         Mat hsv;
         cvtColor(frame,hsv,COLOR_BGR2HSV);
         int histSize[] = {180};
         int channels[] = {0};
         float range_[] = {0, 180};
         const float* range[] = {range_};

         int NumberOfPeople = 0;
         for (People i : DetectedPeople){

             Mat roi, hsv_roi, mask, roi_hist, dst;

             Rect trackedWindows = i.getPos();

             //Region Of Interest
             roi = frame(trackedWindows);

             //Change RGB to HSV
             cvtColor(roi, hsv_roi, COLOR_BGR2HSV);

             //DO inRanger function stored in [mask]
             inRange(hsv_roi, Scalar(0, 60, 32), Scalar(180, 255, 255), mask);
             //bitwise_not(mask,mask);

             //Calc histogram stored in [hist]
             calcHist(&hsv_roi, 1, channels, mask, roi_hist, 1, histSize, range);
             normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);

             //Back projection stored in [dst]
             TermCriteria term_crit(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
             calcBackProject(&hsv, 1, channels, roi_hist, dst, range);

             //CamShift algorithm stored in [rot_rect]
             RotatedRect rot_rect = CamShift(dst, trackedWindows, term_crit);

             Point2f points[4];
             rot_rect.points(points);
             for (int j = 0; j < 4; j++)
                 line(frame, points[j], points[(j+1)%4], 255, 2);

             for(auto j = DetectedContours.begin();j != DetectedContours.end();){
                 const auto tCon = *j;
                 
             }

         }

         for (const auto& tCon : DetectedContours){
             DetectedPeople.emplace_back(People(tCon));
         }

        imshow("DEMO", frame);
        
        
        //while ((char)waitKey(1000) != 'q') break;
        if(waitKey(33)){};
    }

      return 0;
}
