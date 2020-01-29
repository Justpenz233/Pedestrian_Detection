//
// Created by 李斯祺 on 2020/1/28.
//

#include "People.h"
#include <utility>

void People::setPos(Rect pos) {
    Place = std::move(pos);
    updateCenter();
}

Rect People::getPos() {
    return Place;
}

double People::getArea() {
    return Area;
}

Point People::getCenter() {
    return Center;
}

People::People(const CONTOUR& t) {
    Place = boundingRect(t);
    Area = contourArea(t);
    updateCenter();
}

void People::updateCenter() {
    PreCenter = Center;
    Center.x = Place.x + Place.width / 2.0;
    Center.y = Place.y + Place.height / 2.0;
}

People::People(const Rect& t) {
    Place = t;
    Area = t.area();

}

void People::setHist(const Mat& t) {
    roi_hist = t;
}

Mat People::getHist() {
    return roi_hist;
}
