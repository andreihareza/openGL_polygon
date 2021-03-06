#include "CPolygon.hpp"
#include <tuple>
#include <cmath>
#include <limits>
#include <algorithm>
using std::endl;


void CPolygon::init(IOpenGLListener * openGLListener)
{
    std::cout << "CPolygon::" << __func__ << "()" << endl;

    mOpenGLListener = openGLListener;
}

void CPolygon::init(std::istream & file, IOpenGLListener * openGLListener)
{
    std::cout << "CPolygon::" << __func__ << "()" << endl;

    mOpenGLListener = openGLListener;

    readData(file);
    checkIntersections();
}

void CPolygon::readData(std::istream & file)
{
    std::cout << "CPolygon::" << __func__ << "()" << endl;

    /* Read points from file */
    std::vector<CLine::Point> points = readPoints(file);

    /* Starting from second point, add a line between previous and current */
    for(auto i=1u; i<points.size() ; ++i)
    {
        mLines.push_back({points[i-1], points[i]});
    }

    /* Add line between last and first point */
    if (!points.empty())
    {
        mLines.push_back({points.back(), points[0]});
    }
}

std::vector<CLine::Point> CPolygon::readPoints(std::istream & file)
{
    std::cout << "CPolygon::" << __func__ << "()" << endl;

    /* Number of points */
    auto size = 0u;

    file >> size;

    std::vector<CLine::Point> points(size);

    for (auto & point : points)
    {
        file >> point.first >> point.second;
    }

    return points;
}

void CPolygon::checkIntersections()
{
    std::cout << "CPolygon::" << __func__ << "()" << endl;

    mIntersectionPoints.clear();

    for (auto & line : mLines)
    {
        std::vector<CLine::Point> intersectionPoints = line.intersectAny(mLines);
        if (!intersectionPoints.empty())
        {
            /* A line is supposed to be red if it intersects any other line */
            line.setColor(NUtility::EColor::RED);

            /* Add intersection points to list */
            mIntersectionPoints.insert(mIntersectionPoints.end(),
                    intersectionPoints.begin(), intersectionPoints.end());
        }
        else
        {
            /* Otherwise, it is green */
            line.setColor(NUtility::EColor::GREEN);
        }
    }

    /* Remove duplicates from array */
    std::sort(mIntersectionPoints.begin(), mIntersectionPoints.end());
    auto newEnd = std::unique(mIntersectionPoints.begin(), mIntersectionPoints.end());
    mIntersectionPoints.erase(newEnd, mIntersectionPoints.end());

    /* Find all points inside polygon */
    updateInsidePoints();

    /* Update center of mass */
    updateCenterOfMass();
}

void CPolygon::updateInsidePoints()
{
    mPointsInside.clear();

    for (int x=0; x<NUtility::width; ++x)
    {
        for (int y=0; y<NUtility::height; ++y)
        {
            CLine checkedLine {CLine::Point{x, y}, NUtility::exteriorPoint};
            int intersections = checkedLine.intersectAny(mLines).size();
            if (intersections % 2 == 1)
            {
                mPointsInside.push_back(CLine::Point({x, y}));
            }
        }
    }
}

void CPolygon::updateCenterOfMass()
{
    mCenterOfMass = {0, 0};
    for (auto line : mLines)
    {
        auto firstPoint = line.getPoints().first;
        mCenterOfMass.first += firstPoint.first;
        mCenterOfMass.second += firstPoint.second;
    }
    mCenterOfMass.first = mCenterOfMass.first / mLines.size();
    mCenterOfMass.second = mCenterOfMass.second / mLines.size();
}

CLine::Point CPolygon::getCenterOfMass()
{
    return mCenterOfMass;
}

void CPolygon::addPoint(CLine::Point point)
{
    std::cout << "CPolygon::" << __func__ << "()" << endl;

    if (!mLines.empty())
    {
        CLine::Point firstPoint{};
        CLine::Point lastPoint{};
        std::tie(lastPoint, firstPoint) = mLines.back().getPoints();

        /* When we already have a polygon, remove the line between last and first point */
        mLines.pop_back();

        /* Add lines bettwen last point, new added point and first point */
        mLines.push_back({lastPoint, point});
        mLines.push_back({point, firstPoint});
    }
    else
    {
        /* In case of empty polygon, add a line between the only point and itself */
        mLines.push_back({point, point});
    }

    /* Check new polygon for line intersections */
    checkIntersections();

    /* Notify openGL that current polygon has changed */
    mOpenGLListener->notifyPolygonChanged(*this);
}
int CPolygon::findClosest(CLine::Point point, CLine::Point & foundPoint)
{
    std::cout << "CPolygon::" << __func__ << "()" << endl;

    auto square = [](float value)
    {
        return value * value;
    };

    /* Distance between two points */
    auto dist = [&square](CLine::Point a, CLine::Point b)
    {
        return sqrt(square(a.first - b.first) + square(a.second - b.second));
    };

    /* Set minimum to infinite value */
    float minDist = std::numeric_limits<float>::max();
    unsigned int minPos = 0;

    for (auto i=0u; i<mLines.size(); ++i)
    {
        /* Distance between current line's first point and target */
        auto currentDist = dist(point, mLines[i].getPoints().first);

        if (currentDist < minDist)
        {
            minDist = currentDist;
            minPos = i;
            foundPoint = mLines[i].getPoints().first;
        }
    }

    std::cout << "CPolygon::" << __func__ << "(): returning " << minPos << endl;

    return minPos;
}

void CPolygon::editPoint(int pointPos, CLine::Point newPoint)
{
    std::cout << "CPolygon::" << __func__ << "(): pointPos = " << pointPos << endl;

    /* The other line in which current point takes part */
    auto & otherLine = pointPos == 0? mLines.back() : mLines[pointPos-1];

    /* Set new point for both lines containing it */
    mLines[pointPos].editPoint1(newPoint);
    otherLine.editPoint2(newPoint);

    /* Check new polygon for line intersections */
    checkIntersections();

    /* Notify openGL that current polygon has changed */
    mOpenGLListener->notifyPolygonChanged(*this);
}

void CPolygon::deletePoint(int pointPos)
{
    std::cout << "CPolygon::" << __func__ << "(): pointPos = " << pointPos << endl;

    /* The other line in which current point takes part */
    auto & otherLine = pointPos == 0? mLines.back() : mLines[pointPos-1];

    /* The line which should be drawn after point removal */
    CLine newLine = {otherLine.getPoints().first, mLines[pointPos].getPoints().second};

    /* One of the lines will become the new line and the other one will be deleted */
    otherLine = newLine;
    mLines.erase(mLines.begin() + pointPos);

    /* Check new polygon for line intersections */
    checkIntersections();

    /* Notify openGL that current polygon has changed */
    mOpenGLListener->notifyPolygonChanged(*this);
}

std::vector<float> CPolygon::getOpenGLIntersectionPoints()
{
    std::vector<float> coords;

    for (auto currentPoint : mIntersectionPoints)
    {
        /* Coords given to openGL should be a number between -1 and 1 */
        // coords.push_back(currentPoint.first / NUtility::width * 2 - 1);
        // coords.push_back(currentPoint.second / NUtility::height * 2 - 1);
        coords.push_back(currentPoint.first);
        coords.push_back(currentPoint.second);

        /* Polygon is 2d, so 3rd dimension is always 0 */
        if (NUtility::dimensions == 3)
        {
            coords.push_back(0.0f);
        }
        if (NUtility::dimensions == 4)
        {
            coords.insert(coords.end(), {0.0f, 1.0f});
        }
    }

    return coords;
}

std::vector<float> CPolygon::getOpenGLIntersectionPointsColors()
{
    std::vector<float> colors;

    for (auto i=0u; i<mIntersectionPoints.size(); ++i)
    {
        colors.insert(colors.end(), {1.0f, 1.0f, 0.0f, 1.0f});
    }

    return colors;
}

std::vector<float> CPolygon::getOpenGLInsidePoints()
{
    std::vector<float> coords;

    for (auto currentPoint : mPointsInside)
    {
        /* Coords given to openGL should be a number between -1 and 1 */
        // coords.push_back(currentPoint.first / NUtility::width * 2 - 1);
        // coords.push_back(currentPoint.second / NUtility::height * 2 - 1);
        coords.push_back(currentPoint.first);
        coords.push_back(currentPoint.second);

        /* Polygon is 2d, so 3rd dimension is always 0 */
        if (NUtility::dimensions == 3)
        {
            coords.push_back(0.0f);
        }
        if (NUtility::dimensions == 4)
        {
            coords.insert(coords.end(), {0.0f, 1.0f});
        }
    }

    return coords;
}

std::vector<float> CPolygon::getOpenGLInsidePointsColors()
{
    std::vector<float> colors;

    for (auto i=0u; i<mPointsInside.size(); ++i)
    {
        colors.insert(colors.end(), {0.18f, 0.18f, 0.60f, 0.8f});
    }

    return colors;
}

std::size_t CPolygon::getIntersectionPointsNum()
{
    return mIntersectionPoints.size();
}

std::size_t CPolygon::getInsidePointsNum()
{
    return mPointsInside.size();
}

CPolygon::iterator CPolygon::begin()
{
    return mLines.begin();
}

CPolygon::iterator CPolygon::end()
{
    return mLines.end();
}

CPolygon::iterator begin(CPolygon & polygon)
{
    return polygon.begin();
}

CPolygon::iterator end(CPolygon & polygon)
{
    return polygon.end();
}

std::size_t CPolygon::size()
{
    return mLines.size();
}

CLine CPolygon::operator[](std::size_t pos)
{
    return mLines[pos];
}

const CLine CPolygon::operator[](std::size_t pos) const
{
    return mLines[pos];
}

