// Visualizer.h

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <memory>

#include "geometry/Rectangle.h"


namespace Platformer
{


class Visualizer
{
public:
    Visualizer(Visualizer&& other);
    virtual Visualizer& operator=(Visualizer&& other);
    virtual ~Visualizer();

    virtual Rectangle getSceneRect() const = 0;

    virtual void clear() = 0;
    virtual void refresh();
    virtual void drawRect(const Rectangle &rect, bool isMovable, bool isStatic, bool isStand) = 0;
    virtual void setSceneRect(const Rectangle &rect) = 0;

protected:
    Visualizer();

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // VISUALIZER_H
