// CollisionProcessor.cpp

#include "CollisionProcessor.h"


namespace Platformer
{


struct CollisionProcessor::Impl
{
    Impl()
    {
    }

    SimplePhysicalEnginePointer _enginePtr;
};



CollisionProcessor::CollisionProcessor(SimplePhysicalEnginePointer enginePtr)
    : _pimpl(new Impl())
{
    _pimpl->_enginePtr = enginePtr;
}


CollisionProcessor::CollisionProcessor(CollisionProcessor&& /*other*/) = default;
CollisionProcessor& CollisionProcessor::operator=(CollisionProcessor&& /*other*/) = default;
CollisionProcessor::~CollisionProcessor() = default;


SimplePhysicalEnginePointer CollisionProcessor::getEnginePtr() const
{
    if (_pimpl->_enginePtr == nullptr)
        throw std::logic_error("CollisionProcessor::getEnginePtr: engine is not set");

    return _pimpl->_enginePtr;
}


void CollisionProcessor::setEnginePtr(SimplePhysicalEnginePointer enginePtr)
{
    _pimpl->_enginePtr = enginePtr;
}




}  // namespace Platformer
