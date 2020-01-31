//
// Created by 李斯祺 on 2020/1/28.
//

#ifndef PRESIDENT_DETECTION_PEOPLE_H
#define PRESIDENT_DETECTION_PEOPLE_H


#include <vector>
#include <opencv4/opencv2/opencv.hpp>
#include <iostream>
//#include "Parameter.h"
using namespace cv;

typedef std::vector<Point> CONTOUR;

class People {
private:
    double Area;
    Rect ROI;
    Point Center;
    Point PreCenter;


    Mat roi_hist;
    void updateCenter();

public:
    int ID;
    int MissCount;
    Point Speed;
    static int COUNT;
    People(const CONTOUR&);
    People(const Rect&);


    void setHist(const Mat&);
    void setPos(const Rect&);

    void UPDATE(Rect);

    bool JudgeIn(const Rect&);

    Mat getHist();
    Rect getPos();
    double getArea();
    Point getCenter();
};


#endif //PRESIDENT_DETECTION_PEOPLE_H
