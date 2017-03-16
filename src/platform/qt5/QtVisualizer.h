// QtVisualizer.h

#ifndef QTVISUALIZER_H
#define QTVISUALIZER_H

#include <memory>

#include "visualizer/Visualizer.h"


namespace Platformer
{


class QtVisualizer : /*public QGraphicsView,*/ public Visualizer
{
//    Q_OBJECT

public:
    QtVisualizer();
    QtVisualizer(QtVisualizer&& other);
    virtual QtVisualizer& operator=(QtVisualizer&& other);
    virtual ~QtVisualizer();

    virtual Rectangle getSceneRect() const override;

    virtual void clear() override;
    virtual void refresh() override;
    virtual void drawRect(const Rectangle &rect, bool isLight, bool isStatic, bool isStand) override;
    virtual void setSceneRect(const Rectangle &rect) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // QTVISUALIZER_H
