// MapPlatform.cpp

#include "visitor/GameObjectVisitor.h"
#include "MapPlatform.h"


namespace Platformer
{


struct MapPlatform::Impl
{
    Impl()
    {
    }

    Rectangle _rect;
    RectangleIteratorPtr _rectIterPtr;
};



MapPlatform::MapPlatform(Point position,
                         Rectangle rect)
    : _pimpl(new Impl())
{
    setPosition(position);
    _pimpl->_rect = rect;
    _pimpl->_rectIterPtr.reset(new SingleValueIterator<Rectangle>(_pimpl->_rect));
}


MapPlatform::MapPlatform(Rectangle rect)
    : MapPlatform(rect.getPosition(), Rectangle(Point(0, 0), rect.getSize()))
{
}


MapPlatform::MapPlatform(MapPlatform&& /*other*/) = default;
MapPlatform& MapPlatform::operator=(MapPlatform&& /*other*/) = default;
MapPlatform::~MapPlatform() = default;


bool MapPlatform::isMovable() const
{
    return false;
}


RectangleIteratorPtr MapPlatform::getGeometry() const
{
    return _pimpl->_rectIterPtr;
}


void MapPlatform::accept(GameObjectVisitor &visitor)
{
    visitor.visit(*this);
}


}  // namespace Platformer
