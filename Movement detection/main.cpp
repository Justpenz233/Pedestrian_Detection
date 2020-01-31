//
//  main.cpp
//  Peopel_Detection
//
//  Created by 李斯祺 on 2020/1/26.
//  Copyright © 2020 李斯祺. All rights reserved.
//
//

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/video/background_segm.hpp>
#include "People.h"


#define CurrentPath "/Users/makik/Desktop/President_Detection/"

using namespace std;
using namespace cv;

int People::COUNT = 0;

int toleranceRange = 200;  // use for error calculation
int toleranceCount = 10; // maximum number of frame an object need to present in order to be accepted

double MinPeopleArea = 2000;
double MaxPeopleArea = 15000;

list<Rect> DetectedContours;
list<People> DetectedPeople;

string MakePath(const string s) {
    return CurrentPath + s;
}

void NextFram(VideoCapture &cap, double Sec) {
    for (int i = 1; i <= Sec * 30; i++) {
        Mat fram;
        cap >> fram;
    }
}

int main() {

    namedWindow("Frame", WINDOW_AUTOSIZE);
    namedWindow("Mask", WINDOW_AUTOSIZE);
    namedWindow("ROI", WINDOW_AUTOSIZE);

    VideoCapture cap;
    cap.open(MakePath("demo.mp4"));

    Size size = Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
    int myFourCC = VideoWriter::fourcc('m', 'p', '4', 'v');//mp4
    double rate = cap.get(CAP_PROP_FPS);
    VideoWriter writer("Result.mp4", myFourCC, rate, size, true);


    auto fgmask = createBackgroundSubtractorMOG2(100);


    while (cap.isOpened()) {
        Mat frame;
        //NextFram(cap, 0.05);//每0.1秒处理一次

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
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
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

        for (const auto &t : Contours) {
            Rect R = boundingRect(t);
            double tArea = R.area();
            if (tArea < MinPeopleArea or tArea > MaxPeopleArea)
                continue;
            DetectedContours.push_back(boundingRect(t));

        }

        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        int histSize[] = {180};
        int channels[] = {0};
        float range_[] = {0, 180};
        const float *range[] = {range_};

        int NumberOfPeople = 0;
        Mat imFinal;
        copyTo(frame, imFinal, Mat());

        for (auto i = DetectedPeople.begin(); i != DetectedPeople.end();) {

            Mat roi, hsv_roi, mask, roi_hist, dst;

            Rect trackedWindows = i->getPos();

            //Region Of Interest
            roi = frame(trackedWindows);
            imshow("ROI", roi);
            //Change RGB to HSV
            cvtColor(roi, hsv_roi, COLOR_BGR2HSV);

            //DO inRanger function stored in [mask]
            inRange(hsv_roi, Scalar(0, 60, 32), Scalar(180, 255, 255), mask);
            //bitwise_not(mask, mask);
            imshow("Mask", mask);


            //Calc histogram stored in [hist]
            calcHist(&hsv_roi, 1, channels, mask, roi_hist, 1, histSize, range);
            normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);

            //Back projection stored in [dst]
            TermCriteria term_crit(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
            calcBackProject(&hsv, 1, channels, roi_hist, dst, range);

            //CamShift algorithm stored in [rot_rect]
            //RotatedRect rot_rect = CamShift(dst, trackedWindows, term_crit);

            //MeanShift algorithm stored in [trackedWindows]
            meanShift(dst, trackedWindows, term_crit);

            int tx = int(trackedWindows.x + trackedWindows.width / 2.0);
            int ty = int(trackedWindows.y + trackedWindows.height / 2.0);
            Point tPos = Point(tx, ty);

            bool findFlag = false;

            //Matching
            //ToDo (Add tolerance Count)
            for (auto j : DetectedContours) {
                Rect t = j;
                if(
                        tPos.x < int(j.x + j.width + toleranceRange) and
                        tPos.x > int(j.x - toleranceRange) and
                        tPos.y < int(j.y + j.height + toleranceRange) and
                        tPos.y > int(j.y - toleranceRange)
                ){
                    i->UPDATE(j);
                    DetectedContours.remove(j);
                    findFlag = true;
                    break;
                }
            }

           //Deleted People
            if (not findFlag) {
                cout << "Not found" << endl;
                if(i->MissCount > toleranceCount)
                    i = DetectedPeople.erase(i);
                else
                    i->MissCount ++;
                
            }
            else {
                cout << "Found" << endl;
                //Draw result with MeanShift
                rectangle(imFinal, trackedWindows, 255, 2);

                /*
                 * Draw result with CamShift
                 *
                Point2f points[4];
                rot_rect.points(points);
                for (int j = 0; j < 4; j++)
                    line(imFinal, points[j], points[(j + 1) % 4], 255, 2);
                */

                i++;
            }

            for(auto j : DetectedPeople){
                string messg = to_string(j.ID);
                messg = messg + ":";
                Point V = j.Speed;
                string xV = to_string(V.x);
                string yV = to_string(V.y);
                messg = messg + xV + "," + yV;

                Rect t = j.getPos();
                Point pos;
                pos.x = t.x;
                pos.y = t.y + t.height + 5;
                putText(imFinal,messg,pos,FONT_HERSHEY_COMPLEX,0.5,Scalar(0, 255, 255),2);
            }

        }

        for (const auto &tCon : DetectedContours) 
            DetectedPeople.emplace_back(People(tCon));

        writer << imFinal;
        imshow("Frame", imFinal);


        //while ((char)waitKey(1000) != 'q') break;
        if (waitKey(33)) {};
    }
    writer.release();
    return 0;

}
