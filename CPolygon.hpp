#pragma once

#include "CLine.hpp"
#include "IOpenGLListener.hpp"

#include <iostream>

class CPolygon
{
public:
    using iterator = std::vector<CLine>::iterator;

    void init(IOpenGLListener * openGLListener);
    void init(std::istream & file, IOpenGLListener * openGLListener);

    void addPoint(CLine::Point point);

    int findClosest(CLine::Point initialPoint, CLine::Point & foundPoint);
    void editPoint(int pointPos, CLine::Point newPoint);
    void deletePoint(int pointPos);

    std::size_t getIntersectionPointsNum();
    std::size_t getInsidePointsNum();

    std::vector<float> getOpenGLIntersectionPoints();
    std::vector<float> getOpenGLIntersectionPointsColors();

    std::vector<float> getOpenGLInsidePoints();
    std::vector<float> getOpenGLInsidePointsColors();

    CLine::Point getCenterOfMass();

    std::size_t size();

    CLine operator[](std::size_t pos);
    const CLine operator[](std::size_t pos) const;

    iterator begin();
    iterator end();

    friend iterator begin(CPolygon & polygon);
    friend iterator end(CPolygon & polygon);

private:
    void readData(std::istream & file);
    std::vector<CLine::Point> readPoints(std::istream & file);

    void checkIntersections();
    void updateInsidePoints();
    void updateCenterOfMass();

    std::vector<CLine> mLines;
    std::vector<CLine::Point> mIntersectionPoints;

    std::vector<CLine::Point> mPointsInside;

    IOpenGLListener * mOpenGLListener;

    CLine::Point mCenterOfMass;
};

