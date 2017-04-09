// StrictCollisionProcessor.h

#ifndef STRICTCOLLISIONPROCESSOR_H
#define STRICTCOLLISIONPROCESSOR_H

#include <memory>

#include "Types.h"
#include "CollisionProcessor.h"


namespace Platformer
{


class StrictCollisionProcessor : public CollisionProcessor
{
public:
    StrictCollisionProcessor(PhysicalEnginePointer enginePtr = nullptr);
    StrictCollisionProcessor(StrictCollisionProcessor&& other);
    virtual StrictCollisionProcessor& operator=(StrictCollisionProcessor&& other);
    virtual ~StrictCollisionProcessor();

    virtual void processCollisions() override;
    virtual void updateMetadata() override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // STRICTCOLLISIONPROCESSOR_H
