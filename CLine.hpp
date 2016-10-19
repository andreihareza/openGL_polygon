#pragma once
#include <vector>
#include <iostream>
#include "utility.hpp"

class CLine
{
public:
    using Point = std::pair<float, float>;

    CLine(Point p1, Point p2);

    std::vector<Point> intersectAny(std::vector<CLine> & otherLines);

    bool operator==(const CLine & other);
    bool operator!=(const CLine & other);

    void setColor(NUtility::EColor color);

    void printColor(std::ostream & file);

    std::vector<float> getOpenGLFirstPointCoords();
    std::vector<float> getOpenGLSecondPointCoords();
    std::vector<float> getOpenGLPointCoords();

    std::vector<float> getOpenGLFirstPointColors();
    std::vector<float> getOpenGLPointColors();

    std::pair<Point, Point> getPoints();

    void editPoint1(Point newPoint);
    void editPoint2(Point newPoint);

private:
    bool intersect(CLine & other, Point & intersectionPoint);

    Point mPoint1;
    Point mPoint2;

    NUtility::EColor mColor;
};

