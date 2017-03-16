// Rectangle.cpp

#include "Rectangle.h"


namespace Platformer
{


//struct Rectangle::Impl
//{
//    Impl()
//    {
//    }

//    Point _position, _size;
//};



Rectangle::Rectangle()
//    : _pimpl(new Impl())
{
}

Rectangle::Rectangle(Point position, Point size)
    : Rectangle()
{
    setPosition(position);
    setSize(size);
}

Rectangle::Rectangle(double x, double y, double w, double h)
    : Rectangle(Point(x, y), Point(w, h))
{
}

Rectangle::Rectangle(const Rectangle &other)
    : Rectangle(other.getPosition(), other.getSize())
{
}

Rectangle& Rectangle::operator=(const Rectangle& other)
{
    setPosition(other.getPosition());
    setSize(other.getSize());
    return *this;
}

Rectangle::~Rectangle() = default;


Point Rectangle::getPosition() const
{
    return _position;
}

Point Rectangle::getSize() const
{
    return _size;
}

double Rectangle::getHeight() const
{
    return _size.getY();
}

double Rectangle::getWidth() const
{
    return _size.getX();
}

double Rectangle::getTop() const
{
    return _position.getY();
}

double Rectangle::getBottom() const
{
    return _position.getY() + getHeight();
}

double Rectangle::getX() const
{
    return getLeft();
}

double Rectangle::getY() const
{
    return getTop();
}

double Rectangle::getRight() const
{
    return _position.getX() + getWidth();
}

double Rectangle::getLeft() const
{
    return _position.getX();
}

bool Rectangle::isPointInside(const Point &point) const
{
    return (    point.getX() >= getLeft() && point.getX() <= getRight()
                &&  point.getY() >= getTop()  && point.getY() <= getBottom());
}

bool Rectangle::isCollided(const Rectangle &rect) const
{
    return !(rect.getLeft()   > getRight()  ||
             rect.getRight()  < getLeft()   ||
             rect.getTop()    > getBottom() ||
             rect.getBottom() < getTop());
}

bool Rectangle::isStrongCollided(const Rectangle &rect) const
{
    return !(rect.getLeft()   >= getRight()  ||
             rect.getRight()  <= getLeft()   ||
             rect.getTop()    >= getBottom() ||
             rect.getBottom() <= getTop());
}



void Rectangle::setPosition(const Point &position)
{
    _position = position;
}

void Rectangle::setSize(const Point &size)
{
    _size = size;
}

void Rectangle::print(std::ostream &stream) const
{
    stream << "[" << getLeft() << ", " << getTop() << ", " << getRight() << ", " << getBottom() << "]";
}



}  // namespace Platformer
