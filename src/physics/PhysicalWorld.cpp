// PhysicalWorld.cpp

#include "platform/Platform.h"
#include "visualizer/Visualizer.h"
#include "visitor/GameObjectVisitor.h"
#include "PhysicalEngine.h"
#include "PhysicalWorld.h"
#include "physics/MapPlatform.h"


namespace Platformer
{


struct PhysicalWorld::Impl
{
    Impl()
    {
    }

    PhysicalEnginePointer _enginePtr;
};



PhysicalWorld::PhysicalWorld()
    : GameObjectContainer("[PhysicalWorld]")
    , _pimpl(new Impl())
{
}


PhysicalWorld::PhysicalWorld(PhysicalWorld&& /*other*/) = default;
PhysicalWorld& PhysicalWorld::operator=(PhysicalWorld&& /*other*/) = default;
PhysicalWorld::~PhysicalWorld() = default;


PhysicalEnginePointer PhysicalWorld::getEnginePtr() const
{
    return _pimpl->_enginePtr;
}


void PhysicalWorld::accept(GameObjectVisitor &visitor)
{
    visitor.visit(*this);
}


void PhysicalWorld::setEnginePtr(const PhysicalEnginePointer &enginePtr)
{
    _pimpl->_enginePtr = enginePtr;
}


void PhysicalWorld::doAddSubObject(GameObjectPointer /*subObjPtr*/)
{
    if (getEnginePtr() != nullptr)
        getEnginePtr()->updateMetadata();
}


void PhysicalWorld::doRemoveSubObject(GameObjectPointer /*subObjPtr*/)
{
    if (getEnginePtr() != nullptr)
        getEnginePtr()->updateMetadata();
}



}  // namespace Platformer


