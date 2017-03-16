// GameObject.cpp

#include "visitor/GameObjectVisitor.h"
#include "GameObject.h"


namespace Platformer
{


struct GameObject::Impl
{
    Impl()
    {
    }

    SimpleGameObjectPointer _parentPtr = nullptr;
    std::string _name;
};


GameObject::GameObject(const std::string &name)
    : _pimpl(new Impl())
{
    setName(name);
}

GameObject::GameObject(GameObject&& /*other*/) = default;
GameObject& GameObject::operator=(GameObject&& /*other*/) = default;
GameObject::~GameObject() = default;


std::string GameObject::getName() const
{
    return _pimpl->_name;
}

SimpleGameObjectPointer GameObject::getParentPointer()
{
    return _pimpl->_parentPtr;
}

ConstSimpleGameObjectPointer GameObject::getParentPointer() const
{
    return _pimpl->_parentPtr;
}

GameObjectIteratorPtr GameObject::getSubObjects() const
{
    return GameObjectIteratorPtr(new EmptyIterator<GameObjectPointer>());
}

Point GameObject::getPosition() const
{
    return Point(0, 0);
}


Point GameObject::mapToGlobal(const Point &point, SimpleGameObjectPointer parentPtr) const
{
    Point mapedPoint = point;
    ConstSimpleGameObjectPointer objectPtr = this;

    for ( ; objectPtr != nullptr && objectPtr != parentPtr;
          objectPtr = objectPtr->getParentPointer())
        mapedPoint += objectPtr->getPosition();

    return mapedPoint;
}

Rectangle GameObject::mapToGlobal(const Rectangle &rect, SimpleGameObjectPointer parentPtr) const
{
    return Rectangle(mapToGlobal(rect.getPosition(), parentPtr), rect.getSize());
}


void GameObject::setName(const std::string &name)
{
    _pimpl->_name = name;
}

void GameObject::accept(GameObjectVisitor &visitor)
{
    visitor.visit(*this);
}

void GameObject::setParentPointer(const SimpleGameObjectPointer &parentPtr)
{
    _pimpl->_parentPtr = parentPtr;
}





}  // namespace Platformer
