// CollisionProcessor.h

#ifndef COLLISIONPROCESSOR_H
#define COLLISIONPROCESSOR_H

#include <memory>

#include "Types.h"


namespace Platformer
{


class CollisionProcessor
{
public:
    CollisionProcessor(CollisionProcessor&& other);
    virtual CollisionProcessor& operator=(CollisionProcessor&& other);
    virtual ~CollisionProcessor();

    SimplePhysicalEnginePointer getEnginePtr() const;

    void setEnginePtr(SimplePhysicalEnginePointer enginePtr);
    virtual void updateMetadata() = 0;
    virtual void processFrame(double frameTimeSec) = 0;

protected:
    CollisionProcessor(SimplePhysicalEnginePointer enginePtr);

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // COLLISIONPROCESSOR_H
