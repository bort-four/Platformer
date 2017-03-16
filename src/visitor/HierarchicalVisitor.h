// HierarchicalVisitor.h

#ifndef HIERARCHICALVISITOR_H
#define HIERARCHICALVISITOR_H

#include <memory>

#include "Types.h"
#include "GameObjectVisitor.h"


namespace Platformer
{


class HierarchicalVisitor : public GameObjectVisitor
{
public:
    HierarchicalVisitor(HierarchicalVisitor&& other);
    virtual HierarchicalVisitor& operator=(HierarchicalVisitor&& other);
    virtual ~HierarchicalVisitor();

    using GameObjectVisitor::visit;

    virtual void visit(TestObject &) override;
    virtual void visit(MapPlatform&) override;
    virtual void visit(PhysicalObject &) override;
    virtual void visit(PhysicalWorld &) override;
    virtual void visit(GameObjectContainer &) override;
    virtual void visit(GameObject &) override;

protected:
    HierarchicalVisitor();

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // HIERARCHICALVISITOR_H
