//
//  main.cpp
//  Peopel_Detection
//
//  Created by 李斯祺 on 2020/1/26.
//  Copyright © 2020 李斯祺. All rights reserved.
//

#include <iostream>
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

vector <Rect> DetectedContours;
vector <People> DetectedPeople;

vector<int> test;
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
    while (1) {
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
        drawContours(frame, Contours, -1, Scalar(255 ,255, 255));
        
        for (int i = 0;i < Contours.size();i ++){
            const vector<Point> t = Contours[i];
            double tArea = contourArea(t);
            if(tArea < MinPeopelArea and tArea > MaxPeopleArea)
                continue;
            DetectedContours.push_back(boundingRect(t));
        }

        for (int i = 0;i < DetectedPeople.size();i ++){
            
        }



        imshow("Exameple2", frame);
        
        
        //while ((char)waitKey(1000) != 'q') break;
        if(waitKey(33)){};
        
    }

      return 0;
    
    
}
