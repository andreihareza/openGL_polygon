#include "CLine.hpp"
#include "utility.hpp"
using std::endl;

CLine::CLine(Point p1, Point p2)
    : mPoint1(p1)
    , mPoint2(p2)
{
}

bool CLine::operator==(const CLine & other)
{
    return this->mPoint1 == other.mPoint1 &&
           this->mPoint2 == other.mPoint2;
}

bool CLine::operator!=(const CLine & other)
{
    return !(*this == other);
}

std::vector<CLine::Point> CLine::intersectAny(std::vector<CLine> & otherLines)
{
    // std::cout << "CLine::" << __func__ << "()" << endl;

    /* Points where the line intersects something */
    std::vector<Point> intersectionPoints;

    for (auto & line : otherLines)
    {
        Point point;
        if(intersect(line, point))
        {
            intersectionPoints.push_back(point);
        }
    }

    return intersectionPoints;
}

bool CLine::intersect(CLine & other, Point & intersectionPoint)
{
    /* Two lines are not considered to be intersecting if they have a common point */
    if ((mPoint1 == other.mPoint1 ) ||
        (mPoint1 == other.mPoint2 ) ||
        (mPoint2 == other.mPoint1 ) ||
        (mPoint2 == other.mPoint2 ))
    {
        return false;
    }

    auto det3 = [](Point a, Point b, Point c)
    {
        return a.first * b.second
             + b.first * c.second
             + c.first * a.second
             - a.second * b.first
             - b.second * c.first
             - c.second * a.first;
    };

    auto sign = [](float value)
    {
        return value > 0;
    };

    /* For each segment, the points of the other segment should be on differents sides of the initial line */
    if (sign(det3(other.mPoint1, other.mPoint2, this->mPoint1)) !=
            sign(det3(other.mPoint1, other.mPoint2, this->mPoint2))
            &&
           (sign(det3(this->mPoint1, this->mPoint2, other.mPoint1)) !=
            sign(det3(this->mPoint1, this->mPoint2, other.mPoint2))))
    {

        auto det2 = [](float a11, float a12, float a21, float a22)
        {
            return a11 * a22 - a12 * a21;
        };

        /* Line equation ax + by = -c */
        struct lineEq
        {
            float x;
            float y;
            float res;
        };

        lineEq line1{this->mPoint1.second - this->mPoint2.second,
                     this->mPoint2.first - this->mPoint1.first,
                   -(this->mPoint1.first*this->mPoint2.second - this->mPoint2.first*this->mPoint1.second)};

        lineEq line2{other.mPoint1.second - other.mPoint2.second,
                     other.mPoint2.first - other.mPoint1.first,
                   -(other.mPoint1.first*other.mPoint2.second - other.mPoint2.first*other.mPoint1.second)};

        /* Solve equation system to find intersection point */
        float delta = det2(line1.x, line1.y, line2.x, line2.y);
        float deltax = det2(line1.res, line1.y, line2.res, line2.y);
        float deltay = det2(line1.x, line1.res, line2.x, line2.res);

        intersectionPoint.first = deltax / delta;
        intersectionPoint.second = deltay / delta;

        return true;
    }
    else
    {
        return false;
    }
}

void CLine::setColor(NUtility::EColor color)
{
    mColor = color;
}

void CLine::printColor(std::ostream & file)
{
    std::cout << "CLine::" << __func__ << "(): Point1 is: (" << mPoint1.first << "," << mPoint1.second << ")" << ", ";
    std::cout << "Point2 is: (" << mPoint2.first << "," << mPoint2.second << "): ";
    switch (mColor)
    {
        case NUtility::EColor::GREEN:
            file << "Green" << endl;
            break;
        case NUtility::EColor::RED:
            file << "Red" << endl;
            break;
        default:
            // TODO exeption handling
            file << "Undefined" << endl;
            throw 0;
            break;
    }
}

bool operator==(CLine::Point a, CLine::Point b)
{
    /* Two floating number are considered equal if the difference between them is lower than a small error number */
    auto equal = [](float a, float b)
    {
        auto dif = a - b;
        dif = dif < 0? -dif : dif;
        return dif < NUtility::err;
    };

    return equal(a.first, b.first) && equal(a.second, b.second);
}

bool operator!=(CLine::Point a, CLine::Point b)
{
    return !(a == b);
}

std::vector<float> CLine::getOpenGLFirstPointCoords()
{
    std::vector<float> coords;
    /* Coords given to openGL should be a number between -1 and 1 */
    // coords.push_back(mPoint1.first / NUtility::width * 2 - 1);
    // coords.push_back(mPoint1.second / NUtility::height * 2 - 1);
    coords.push_back(mPoint1.first);
    coords.push_back(mPoint1.second);

    /* Polygon is 2d, so 3rd dimension is always 0 */
    if (NUtility::dimensions == 3)
    {
        coords.push_back(0.0f);
    }
    if (NUtility::dimensions == 4)
    {
        coords.insert(coords.end(), {0.0f, 1.0f});
    }

    return coords;
}

std::vector<float> CLine::getOpenGLSecondPointCoords()
{
    std::vector<float> coords;
    /* Coords given to openGL should be a number between -1 and 1 */
    // coords.push_back(mPoint2.first / NUtility::width * 2 - 1);
    // coords.push_back(mPoint2.second / NUtility::height * 2 - 1);
    coords.push_back(mPoint2.first);
    coords.push_back(mPoint2.second);

    /* Polygon is 2d, so 3rd dimension is always 0 */
    if (NUtility::dimensions == 3)
    {
        coords.push_back(0.0f);
    }
    if (NUtility::dimensions == 4)
    {
        coords.insert(coords.end(), {0.0f, 1.0f});
    }

    return coords;
}

std::vector<float> CLine::getOpenGLPointCoords()
{
    std::vector<float> coords;

    auto firstPoint = getOpenGLFirstPointCoords();
    auto secondPoint = getOpenGLSecondPointCoords();

    coords.insert(coords.end(), firstPoint.begin(), firstPoint.end());
    coords.insert(coords.end(), secondPoint.begin(), secondPoint.end());

    return coords;
}

std::vector<float> CLine::getOpenGLFirstPointColors()
{
    std::vector<float> colors;

    colors.insert(colors.end(), {0.0f, 0.0f, 1.0f, 1.0f});

    return colors;
}

std::vector<float> CLine::getOpenGLPointColors()
{
    std::vector<float> colors;

    switch (mColor)
    {
        case NUtility::EColor::GREEN:
        {
            /* Both points are green */
            colors.insert(colors.end(), {0.0f, 1.0f, 0.0f, 1.0f});
            colors.insert(colors.end(), {0.0f, 1.0f, 0.0f, 1.0f});
            break;
        }
        case NUtility::EColor::RED:
        {
            /* Both points are red */
            colors.insert(colors.end(), {1.0f, 0.0f, 0.0f, 1.0f});
            colors.insert(colors.end(), {1.0f, 0.0f, 0.0f, 1.0f});
            break;
        }
        default:
            // TODO Exception handling
            throw 0;
            break;
    }

    return colors;
}

std::pair<CLine::Point, CLine::Point> CLine::getPoints()
{
    return {mPoint1, mPoint2};
}

void CLine::editPoint1(Point newPoint)
{
    mPoint1 = newPoint;
}

void CLine::editPoint2(Point newPoint)
{
    mPoint2 = newPoint;
}

