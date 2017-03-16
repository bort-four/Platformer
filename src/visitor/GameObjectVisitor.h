// GameObjectVisitor.h

#ifndef GAMEOBJECTVISITOR_H
#define GAMEOBJECTVISITOR_H

#include "VisitorBase.h"


namespace Platformer
{


class GameObjectVisitor : public VisitorBase<GameObject>
{
public:
    GameObjectVisitor();
    GameObjectVisitor(GameObjectVisitor&& other);
    virtual GameObjectVisitor& operator=(GameObjectVisitor&& other);
    virtual ~GameObjectVisitor();

    using VisitorBase<GameObject>::visit;

    virtual void visit(TestObject& /*node*/) {}
    virtual void visit(MapPlatform& /*node*/) {}
    virtual void visit(PhysicalObject& /*node*/) {}
    virtual void visit(PhysicalWorld& /*node*/) {}
    virtual void visit(GameObjectContainer& /*node*/) {}
    virtual void visit(GameObject& /*node*/) {}

protected:
    virtual void doProcess(GameObject&node) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // GAMEOBJECTVISITOR_H
