// GameObjectContainer.cpp

#include <vector>
#include <algorithm>

#include "Iterator.h"
#include "visitor/GameObjectVisitor.h"
#include "GameObjectContainer.h"


namespace Platformer
{


struct GameObjectContainer::Impl
{
    Impl()
    {
    }

    std::vector<GameObjectPointer> _subObjects;
};



GameObjectContainer::GameObjectContainer(const std::string &name)
    : GameObject(name)
    , _pimpl(new Impl())
{
}

GameObjectContainer::GameObjectContainer(GameObjectContainer&& /*other*/) = default;
GameObjectContainer& GameObjectContainer::operator=(GameObjectContainer&& /*other*/) = default;
GameObjectContainer::~GameObjectContainer()
{
    clearSubObjects();
}


GameObjectIteratorPtr GameObjectContainer::getSubObjects() const
{
    return createContainerIterator(_pimpl->_subObjects);
}


bool GameObjectContainer::isSubObject(GameObjectPointer subObjPtr) const
{
    return subObjPtr->getParentPointer() == this;

//    return std::find(_pimpl->_subObjects.begin(), _pimpl->_subObjects.end(), subObjPtr)
//            != _pimpl->_subObjects.end();
}

void GameObjectContainer::addSubObject(GameObjectPointer subObjPtr)
{
    if (isSubObject(subObjPtr))
        return;

    _pimpl->_subObjects.push_back(subObjPtr);
    subObjPtr->setParentPointer(this);

    doAddSubObject(subObjPtr);
}

void GameObjectContainer::removeSubObject(GameObjectPointer subObjPtr)
{
    if (!isSubObject(subObjPtr))
        throw std::logic_error("GameObjectContainer::removeSubObject: object isn't in container");

    auto it = std::find(_pimpl->_subObjects.begin(), _pimpl->_subObjects.end(), subObjPtr);

    if (it == _pimpl->_subObjects.end())
        throw std::logic_error("GameObjectContainer::removeSubObject: object isn't in container");

    (*it)->setParentPointer(this);
    _pimpl->_subObjects.erase(it);

    doRemoveSubObject(subObjPtr);
}


void GameObjectContainer::clearSubObjects()
{
    try
    {
        for ( ; !_pimpl->_subObjects.empty(); )
            removeSubObject(_pimpl->_subObjects.front());
    }
    catch (...)
    {
    }
}


void GameObjectContainer::accept(GameObjectVisitor &visitor)
{
    visitor.visit(*this);
}


void GameObjectContainer::doAddSubObject(GameObjectPointer /*subObjPtr*/)
{
}

void GameObjectContainer::doRemoveSubObject(GameObjectPointer /*subObjPtr*/)
{
}


}  // namespace Platformer
