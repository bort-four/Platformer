// Rectangle.h

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <memory>
#include <ostream>

#include "Types.h"
#include "Point.h"


namespace Platformer
{


class Rectangle
{
public:
    Rectangle();
    Rectangle(Point position, Point size);
    Rectangle(double x, double y, double w, double h);
    Rectangle(const Rectangle& other);
    Rectangle& operator=(const Rectangle& other);
    ~Rectangle();

    Point getPosition() const;
    Point getSize() const;
    double getHeight() const;
    double getWidth() const;
    double getTop() const;
    double getBottom() const;
    double getX() const;
    double getY() const;
    double getRight() const;
    double getLeft() const;
    bool isPointInside(const Point &point) const;
    bool isCollided(const Rectangle& rect) const;
    bool isStrongCollided(const Rectangle& rect) const;

    void setPosition(const Point &position);
    void setSize(const Point &size);

    void print(std::ostream &stream) const;

private:
    Point _position, _size;

//    struct Impl;
//    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // RECTANGLE_H
