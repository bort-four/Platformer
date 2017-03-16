// MapPlatform.h

#ifndef MAPPLATFORM_H
#define MAPPLATFORM_H

#include <memory>

#include "PhysicalObject.h"


namespace Platformer
{


class MapPlatform : public PhysicalObject
{
public:
    MapPlatform(Point position = Point(),
                Rectangle rect = Rectangle(0, 0, 150, 50));
    MapPlatform(Rectangle rect);
    MapPlatform(MapPlatform&& other);
    virtual MapPlatform& operator=(MapPlatform&& other);
    virtual ~MapPlatform();

    virtual bool isMovable() const override;
    virtual RectangleIteratorPtr getGeometry() const override;

    virtual void accept(GameObjectVisitor &visitor) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // MAPPLATFORM_H
