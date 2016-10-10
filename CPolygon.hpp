#pragma once

#include "CLine.hpp"
#include "IOpenGLListener.hpp"

#include <iostream>

class CPolygon
{
public:
    void init(IOpenGLListener * openGLListener);
    void init(std::istream & file, IOpenGLListener * openGLListener);

    void addPoint(CLine::Point point);

    std::size_t size();

    CLine operator[](std::size_t pos);
    const CLine operator[](std::size_t pos) const;

    using iterator = std::vector<CLine>::iterator;
    iterator begin();
    iterator end();

    friend iterator begin(CPolygon & polygon);
    friend iterator end(CPolygon & polygon);

private:
    void readData(std::istream & file);
    std::vector<CLine::Point> readPoints(std::istream & file);

    void checkIntersections();

    std::vector<CLine> mLines;

    IOpenGLListener * mOpenGLListener;
};

