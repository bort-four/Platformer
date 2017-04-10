// PhysicalObject.h

#ifndef PHYSICALOBJECT_H
#define PHYSICALOBJECT_H

#include "geometry/Rectangle.h"
#include "game_object/GameObjectContainer.h"


namespace Platformer
{


class PhysicalObject : public GameObjectContainer
{
public:
    PhysicalObject(PhysicalObject&& other);
    virtual PhysicalObject& operator=(PhysicalObject&& other);
    virtual ~PhysicalObject();

    virtual double getMass() const;
    virtual double getFrictionFactor() const;
    virtual double getHitRecoveryFactor() const;
    virtual bool isMovable() const;
    virtual Point getPosition() const override;
    virtual Point getSpeed() const;
    virtual RectangleIteratorPtr getGeometry() const;
    virtual SimplePhysicalObjectPointer getContiguousObject(Direction dir) const;

    virtual void accept(GameObjectVisitor& visitor) override;
    virtual void setPosition(Point posotion);
    virtual void setSpeed(const Point &speed);
    virtual void setContiguousObject(Direction dir, SimplePhysicalObjectPointer objectPtr);
    virtual void resetContiguousObjects();

protected:
    PhysicalObject(const std::string &name = "[PhysicalObject]");

private:

    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // PHYSICALOBJECT_H
