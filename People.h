//
// Created by 李斯祺 on 2020/1/28.
//

#ifndef PRESIDENT_DETECTION_PEOPLE_H
#define PRESIDENT_DETECTION_PEOPLE_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

typedef std::vector<Point> CONTOUR;

class People {
private:
    double Area;
    Rect Place;
    Point Center;
    Point PreCenter;
    Mat roi_hist;
    void updateCenter();
public:
    People(const CONTOUR&);
    People(const Rect&);

    void setHist(const Mat&);
    void setPos(Rect);

    Mat getHist();
    Rect getPos();
    double getArea();
    Point getCenter();
};


#endif //PRESIDENT_DETECTION_PEOPLE_H
