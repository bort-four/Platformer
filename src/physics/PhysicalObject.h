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
    virtual double getEnergyLostFactor() const;
    virtual bool isMovable() const;
//    virtual bool isStatic() const;
//    virtual bool isStand() const;
    virtual Point getPosition() const override;
    virtual Point getSpeed() const;
    virtual RectangleIteratorPtr getGeometry() const;
    virtual SimplePhysicalObjectPointer getContiguousObject(Direction dir) const;
//    virtual size_t getStaticFrameCount() const;
//    virtual Point getLastPosition() const;

    virtual void accept(GameObjectVisitor& visitor) override;
    virtual void setPosition(Point posotion);
    virtual void setSpeed(const Point &speed);
    virtual void setContiguousObject(Direction dir, SimplePhysicalObjectPointer objectPtr);
    virtual void resetContiguousObjects();
//    virtual void setIsStatic(bool isStatic);
//    virtual void setIsStand(bool isStand);
//    virtual void setStaticFrameCount(size_t count);
//    virtual void setLastPosition(const Point &position);

protected:
    PhysicalObject(const std::string &name = "[PhysicalObject]");

private:

    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // PHYSICALOBJECT_H
