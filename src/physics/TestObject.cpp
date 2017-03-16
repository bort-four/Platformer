// TestObject.cpp

#include <vector>

#include "geometry/Rectangle.h"
#include "visitor/GameObjectVisitor.h"
#include "TestObject.h"


namespace Platformer
{


struct TestObject::Impl
{
    Impl()
    {
    }

    double _mass = 0;
    std::vector<Rectangle> _geometry;
};



TestObject::TestObject(double mass, const std::string &name)
    : PhysicalObject(name)
    , _pimpl(new Impl())
{
    setMass(mass);
    //setRectangle(Rectangle(0, 0, 170, 100));

    _pimpl->_geometry.push_back(Rectangle(0, 0, 70, 70));

//    auto h = 50;
//    auto l = 5;

//    _pimpl->_geometry.push_back(Rectangle(-h - l, -h - l, h, h));
//    _pimpl->_geometry.push_back(Rectangle(     l, -h - l, h, h));
//    _pimpl->_geometry.push_back(Rectangle(-h - l,      l, h, h));
//    _pimpl->_geometry.push_back(Rectangle(     l,      l, h, h));
}


TestObject::TestObject(const std::string &name)
    : TestObject(10, name)
{
}


TestObject::TestObject(TestObject&& /*other*/) = default;
TestObject& TestObject::operator=(TestObject&& /*other*/) = default;
TestObject::~TestObject() = default;


double TestObject::getMass() const
{
    return _pimpl->_mass;
}

RectangleIteratorPtr TestObject::getGeometry() const
{
//    return IteratorPointer<Rectangle>(
//                new SingleValueIterator<Rectangle>(getRectangle()));

    return createContainerIterator(_pimpl->_geometry);
}

//const Rectangle &TestObject::getRectangle() const
//{
//    return _pimpl->_rect;
//}

void TestObject::setMass(double mass)
{
    _pimpl->_mass = mass;
}

void TestObject::setSize(const Point &size)
{
    _pimpl->_geometry.front().setSize(size);
}

//void TestObject::setRectangle(const Rectangle &rect)
//{
//    _pimpl->_rect = rect;
//}

void TestObject::accept(GameObjectVisitor &visitor)
{
    visitor.visit(*this);
}


}  // namespace Platformer


