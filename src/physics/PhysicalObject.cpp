// PhysicalObject.cpp

#include "visitor/GameObjectVisitor.h"
#include "PhysicalObject.h"
#include "PhysicalEngine.h"


namespace Platformer
{


struct PhysicalObject::Impl
{
    Impl()
    {
    }

    Point _position;
    Point _speed;
    SimplePhysicalObjectPointer _contiguousObjects[4] = {nullptr, nullptr, nullptr, nullptr};
    bool _isStatic = false;
    bool _isStand = false;
    size_t _staticFrameCount = 0;
    Point _lastPosition;
};



PhysicalObject::PhysicalObject(const std::string &name)
    : GameObjectContainer(name)
    , _pimpl(new Impl())
{
}



PhysicalObject::PhysicalObject(PhysicalObject&& /*other*/) = default;
PhysicalObject& PhysicalObject::operator=(PhysicalObject&& /*other*/) = default;
PhysicalObject::~PhysicalObject() = default;


double PhysicalObject::getMass() const
{
    return 0;
}

double PhysicalObject::getFrictionFactor() const
{
    return PhysicalEngine::getDefaultFirictionFactor();
}

double PhysicalObject::getHitRecoveryFactor() const
{
    return PhysicalEngine::getDefaultHitRecoveryFactor();
}

bool PhysicalObject::isMovable() const
{
    return true;
}

//bool PhysicalObject::isStatic() const
//{
//    return _pimpl->_isStatic;
//}

//bool PhysicalObject::isStand() const
//{
//    return _pimpl->_isStand;
//}

Point PhysicalObject::getPosition() const
{
    return _pimpl->_position;
}

Point PhysicalObject::getSpeed() const
{
    return _pimpl->_speed;
}

RectangleIteratorPtr PhysicalObject::getGeometry() const
{
    return IteratorPointer<Rectangle>(new EmptyIterator<Rectangle>());
}

SimplePhysicalObjectPointer PhysicalObject::getContiguousObject(Direction dir) const
{
    return _pimpl->_contiguousObjects[dir];
}

//size_t PhysicalObject::getStaticFrameCount() const
//{
//    return _pimpl->_staticFrameCount;
//}

//Point PhysicalObject::getLastPosition() const
//{
//    return _pimpl->_lastPosition;
//}

//Point PhysicalObject::mapToGlobal(const Point &point) const
//{
//    Point mapedPoint = point;
//    ConstSimpleGameObjectPointer objectPtr = this;

//    for ( ; objectPtr != nullptr; objectPtr = objectPtr->getParentPointer())
//        mapedPoint += objectPtr->getPosition();

//    return mapedPoint;
//}


//Rectangle PhysicalObject::mapToGlobal(const Rectangle &rect) const
//{
//    return Rectangle(mapToGlobal(rect.getPosition()), rect.getSize());
//}


void PhysicalObject::accept(GameObjectVisitor &visitor)
{
    visitor.visit(*this);
}

void PhysicalObject::setPosition(Point posotion)
{
    _pimpl->_position = posotion;
}

void PhysicalObject::setSpeed(const Point &speed)
{
    _pimpl->_speed = speed;
}

void PhysicalObject::setContiguousObject(Direction dir, SimplePhysicalObjectPointer objectPtr)
{
    _pimpl->_contiguousObjects[dir] = objectPtr;
}

void PhysicalObject::resetContiguousObjects()
{
    for (long num : Range(4))
        _pimpl->_contiguousObjects[num] = nullptr;
}

//void PhysicalObject::setIsStatic(bool isStatic)
//{
//    _pimpl->_isStatic = isStatic;
//}

//void PhysicalObject::setIsStand(bool isStand)
//{
//    _pimpl->_isStand = isStand;
//}

//void PhysicalObject::setStaticFrameCount(size_t count)
//{
//    _pimpl->_staticFrameCount = count;
//}

//void PhysicalObject::setLastPosition(const Point &position)
//{
//    _pimpl->_lastPosition = position;
//}


}  // namespace Platformer
