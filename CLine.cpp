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

bool CLine::intersectAny(std::vector<CLine> & otherLines)
{
    // std::cout << "CLine::" << __func__ << "()" << endl;
    for (auto & line : otherLines)
    {
        if(intersect(line))
        {
            return true;
        }
    }

    return false;
}

bool CLine::intersect(CLine & other)
{
    /* Two lines are not considered to be intersecting if they have a common point */
    if ((mPoint1 == other.mPoint1 ) ||
        (mPoint1 == other.mPoint2 ) ||
        (mPoint2 == other.mPoint1 ) ||
        (mPoint2 == other.mPoint2 ))
    {
        return false;
    }

    auto det = [](Point a, Point b, Point c)
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
    return (sign(det(other.mPoint1, other.mPoint2, this->mPoint1)) !=
            sign(det(other.mPoint1, other.mPoint2, this->mPoint2)))
            &&
           (sign(det(this->mPoint1, this->mPoint2, other.mPoint1)) !=
            sign(det(this->mPoint1, this->mPoint2, other.mPoint2)));
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

std::vector<float> CLine::getOpenGLPointCoords()
{
    /* Coords given to openGL should be a number between -1 and 1 */
    std::vector<float> coords;
    coords.push_back(mPoint1.first / NUtility::width * 2 - 1);
    coords.push_back(mPoint1.second / NUtility::height * 2 - 1);

    /* Polygon is 2d, so 3rd dimension is always 0 */
    if (NUtility::dimensions == 3)
    {
        coords.push_back(0.0f);
    }
    if (NUtility::dimensions == 4)
    {
        coords.insert(coords.end(), {0.0f, 1.0f});
    }


    /* Coords given to openGL should be a number between -1 and 1 */
    coords.push_back(mPoint2.first / NUtility::width * 2 - 1);
    coords.push_back(mPoint2.second / NUtility::height * 2 - 1);

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

std::vector<float> CLine::getOpenGLPointColors()
{
    std::vector<float> colors;

    switch (mColor)
    {
        case NUtility::EColor::GREEN:
        {
            /* Both points are green */
            colors.insert(colors.end(), {0.0f, 1.0f, 0.0f, 0.0f});
            colors.insert(colors.end(), {0.0f, 1.0f, 0.0f, 0.0f});
            break;
        }
        case NUtility::EColor::RED:
        {
            /* Both points are red */
            colors.insert(colors.end(), {1.0f, 0.0f, 0.0f, 0.0f});
            colors.insert(colors.end(), {1.0f, 0.0f, 0.0f, 0.0f});
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

