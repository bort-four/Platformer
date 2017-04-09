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

    PhysicalEnginePointer getEnginePtr() const;

    void setEnginePtr(const PhysicalEnginePointer &getEnginePtr);
    virtual void processCollisions() = 0;
    virtual void updateMetadata() = 0;

protected:
    CollisionProcessor(PhysicalEnginePointer enginePtr);

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // COLLISIONPROCESSOR_H
