// CollisionProcessor.cpp

#include "CollisionProcessor.h"


namespace Platformer
{


struct CollisionProcessor::Impl
{
    Impl()
    {
    }

    PhysicalEnginePointer _enginePtr;
};



CollisionProcessor::CollisionProcessor(PhysicalEnginePointer enginePtr)
    : _pimpl(new Impl())
{
    _pimpl->_enginePtr = enginePtr;
}


CollisionProcessor::CollisionProcessor(CollisionProcessor&& /*other*/) = default;
CollisionProcessor& CollisionProcessor::operator=(CollisionProcessor&& /*other*/) = default;
CollisionProcessor::~CollisionProcessor() = default;


PhysicalEnginePointer CollisionProcessor::getEnginePtr() const
{
    if (_pimpl->_enginePtr == nullptr)
        throw std::logic_error("CollisionProcessor::getEnginePtr: engine is not set");

    return _pimpl->_enginePtr;
}


void CollisionProcessor::setEnginePtr(const PhysicalEnginePointer &enginePtr)
{
    _pimpl->_enginePtr = enginePtr;
}




}  // namespace Platformer
