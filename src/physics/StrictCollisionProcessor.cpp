// StrictCollisionProcessor.cpp

#include <assert.h>
#include <vector>

#include "Iterator.h"
#include "geometry/Point.h"
#include "PhysicalObject.h"
#include "PhysicalWorld.h"
#include "PhysicalEngine.h"
#include "visitor/SimpleHierarchicalVisitor.h"
#include "StrictCollisionProcessor.h"


namespace Platformer
{


struct ObjectMetadata
{
    ObjectMetadata(SimplePhysicalObjectPointer objectPtr)
        : _objectPtr(objectPtr)
    {
        assert(objectPtr != nullptr);
        _lastPosition = objectPtr->getPosition();
    }

    SimplePhysicalObjectPointer _objectPtr = nullptr;
    ObjectMetadata *_contiguousObjects[4] = {nullptr, nullptr, nullptr, nullptr};
    Point _lastPosition;
    size_t _staticFrameCount = 0;
    bool _isStatic = false;
    bool _isStand = false;
};



struct StrictCollisionProcessor::Impl
{
    Impl()
    {
    }

    SimpleHierarchicalVisitorPointer<PhysicalObject> _objectCollectorPtr;
    std::vector<ObjectMetadata> _objectVect;
};


//struct CollisionInfo
//{
//    size_t _lessObjectNum, _largerObectNum;
//    double _time;
//    bool _hasCollision;
//    Direction _direction;
//};



StrictCollisionProcessor::StrictCollisionProcessor(PhysicalEnginePointer enginePtr)
    : CollisionProcessor(enginePtr)
    , _pimpl(new Impl())
{
    _pimpl->_objectCollectorPtr.reset(new SimpleHierarchicalVisitor<PhysicalObject>
                                      ([this](PhysicalObject &object)
    {
        _pimpl->_objectVect.emplace_back(&object);
    }));
}

StrictCollisionProcessor::StrictCollisionProcessor(StrictCollisionProcessor&& /*other*/) = default;
StrictCollisionProcessor& StrictCollisionProcessor::operator=(StrictCollisionProcessor&& /*other*/) = default;
StrictCollisionProcessor::~StrictCollisionProcessor() = default;


void StrictCollisionProcessor::processCollisions()
{
}


void StrictCollisionProcessor::updateMetadata()
{
    if (getEnginePtr()->getWorldPtr() == nullptr)
        throw std::logic_error("PhysicalEngine::updateObjectCache: world is not set");

    _pimpl->_objectVect.clear();
    _pimpl->_objectCollectorPtr->visit(getEnginePtr()->getWorldPtr()->getSubObjects());
}




}  // namespace Platformer
