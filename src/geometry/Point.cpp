// Point.cpp

#include <cmath>

#include "Point.h"


namespace Platformer
{


//struct Point::Impl
//{
//    Impl()
//    {
//    }

//    double _x, _y;
//};



Point::Point(double x, double y)
//    : _pimpl(new Impl())
{
    setX(x);
    setY(y);
}

Point::Point(Direction dir)
    : Point(0, 0)
{
    setX((dir == Left) ? -1 : ((dir = Right) ? 1 : 0));
    setX((dir == Up)   ? -1 : ((dir = Down)  ? 1 : 0));
}

Point::Point(const Point &other)
    : Point(other.getX(), other.getY())
{
}

Point &Point::operator=(const Point &other)
{
    setX(other.getX());
    setY(other.getY());

    return *this;
}

double Point::getLength() const
{
    return std::sqrt(_x * _x + _y * _y);
}

double Point::getProjection(bool isHorisontal) const
{
    if (isHorisontal)
         return getX();
    else return getY();
}


void Point::setProjection(double value, bool isHorisontal)
{
    if (isHorisontal)
         setX(value);
    else setY(value);
}

Point::~Point() = default;


Point Point::operator+(const Point &pt)
{
    return Point(getX() + pt.getX(), getY() + pt.getY());
}

Point Point::operator+(double val)
{
    return Point(getX() + val, getY() + val);
}

Point Point::operator*(double val)
{
    return Point(getX() * val, getY() * val);
}

Point Point::operator/(double val)
{
    return Point(getX() / val, getY() / val);
}

Point &Point::operator+=(const Point &pt)
{
    setX(getX() + pt.getX());
    setY(getY() + pt.getY());
    return *this;
}

Point &Point::operator+=(double val)
{
    setX(getX() + val);
    setY(getY() + val);
    return *this;
}

Point &Point::operator*=(double val)
{
    setX(getX() * val);
    setY(getY() * val);
    return *this;
}

Point &Point::operator/=(double val)
{
    setX(getX() / val);
    setY(getY() / val);
    return *this;
}


Direction getOppositeDirrection(Direction dir)
{
    switch (dir)
    {
    case Down:  return Up;
    case Up:    return Down;
    case Left:  return Right;
    case Right: return Left;
    }

    throw std::logic_error("getOppositeDirrection: invalid direction");
}



}  // namespace Platformer
