// PhysicalWorld.h

#ifndef PHYSICALWORLD_H
#define PHYSICALWORLD_H

#include "game_object/GameObjectContainer.h"


namespace Platformer
{


class PhysicalWorld : public GameObjectContainer
{
public:
    PhysicalWorld();
    PhysicalWorld(PhysicalWorld&& other);
    virtual PhysicalWorld& operator=(PhysicalWorld&& other);
    virtual ~PhysicalWorld();

    PhysicalEnginePointer getEnginePtr() const;

    virtual void accept(GameObjectVisitor& visitor) override;
    void setEnginePtr(const PhysicalEnginePointer &enginePtr);

protected:
    virtual void doAddSubObject(GameObjectPointer subObjPtr) override;
    virtual void doRemoveSubObject(GameObjectPointer subObjPtr) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // PHYSICALWORLD_H
