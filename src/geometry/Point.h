// Point.h

#ifndef POINT_H
#define POINT_H

#include <memory>


namespace Platformer
{

enum Direction {Left, Right, Up, Down};


Direction getOppositeDirrection(Direction dir);


class Point
{
public:
    Point(double x = 0, double y = 0);
    Point(Direction dir);
    Point(const Point &other);
    Point& operator=(const Point& other);
    ~Point();

    inline double getX() const { return _x; }
    inline double getY() const { return _y; }
    double getLength() const;

    inline void setX(double x) { _x = x; }
    inline void setY(double y) { _y = y; }

    Point operator+(const Point &pt);
    Point operator+(double val);
    Point operator*(double val);
    Point operator/(double val);
    Point& operator+=(const Point &pt);
    Point& operator+=(double val);
    Point& operator*=(double val);
    Point& operator/=(double val);

private:
    double _x, _y;

//    struct Impl;
//    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // POINT_H
