// GamePainter.h

#ifndef GAMEPAINTER_H
#define GAMEPAINTER_H

#include <memory>

#include "Types.h"
//#include "GameObjectVisitor.h"
#include "HierarchicalVisitor.h"


namespace Platformer
{


class GamePainter : public HierarchicalVisitor /*GameObjectVisitor*/
{
public:
    GamePainter();
    GamePainter(GamePainter&& other);
    virtual GamePainter& operator=(GamePainter&& other);
    virtual ~GamePainter();

    using GameObjectVisitor::visit;

    virtual void visit(PhysicalObject &node) override;

//    virtual void visit(TestObject &) override;
//    virtual void visit(PhysicalWorld &) override;

protected:
    virtual void doPreprocessAll() override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // GAMEPAINTER_H
