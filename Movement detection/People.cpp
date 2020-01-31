//
// Created by 李斯祺 on 2020/1/28.
//

#include "People.h"
#include <utility>
#include <iostream>

void People::setPos(const Rect& pos) {
    std::cout << COUNT << std::endl;
    ROI = pos;
    updateCenter();
}

Rect People::getPos() {
    return ROI;
}

double People::getArea() {
    return Area;
}

Point People::getCenter() {
    return Center;
}

People::People(const CONTOUR& t) {
    MissCount = 0;
    ID = ++ COUNT;
    std::cout << COUNT << std::endl;
    Center = Point(0,0);
    ROI = boundingRect(t);
    Area = contourArea(t);
    updateCenter();
}

void People::updateCenter() {
    PreCenter = Center;
    Center.x = ROI.x + ROI.width / 2.0;
    Center.y = ROI.y + ROI.height / 2.0;
    Speed = Center - PreCenter;
}

People::People(const Rect& t) {
    MissCount = 0;
    ID = ++ COUNT;
    ROI = t;
    Area = t.area();

}

void People::setHist(const Mat& t) {
    roi_hist = t;
}

Mat People::getHist() {
    return roi_hist;
}

bool People::JudgeIn(const Rect& R) {
    Rect t1 = R & ROI;
    return (t1.area() * 3.0 >= ROI.area()) or (t1.area() * 1.5 >= R.area());
}

void People::UPDATE(Rect R) {
    ROI = R;
    Area = R.area();
    updateCenter();
}
