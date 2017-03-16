// HierarchicalVisitor.cpp

#include "game_object/GameObjectContainer.h"
#include "physics/TestObject.h"
#include "physics/PhysicalWorld.h"
#include "physics/MapPlatform.h"

#include "HierarchicalVisitor.h"


namespace Platformer
{


struct HierarchicalVisitor::Impl
{
    Impl()
    {
    }
};



HierarchicalVisitor::HierarchicalVisitor()
    : _pimpl(new Impl())
{
}

HierarchicalVisitor::HierarchicalVisitor(HierarchicalVisitor&& /*other*/) = default;
HierarchicalVisitor& HierarchicalVisitor::operator=(HierarchicalVisitor&& /*other*/) = default;
HierarchicalVisitor::~HierarchicalVisitor() = default;


void HierarchicalVisitor::visit(TestObject &node)
{
    visit(static_cast<PhysicalObject&>(node));
}

void HierarchicalVisitor::visit(MapPlatform &node)
{
    visit(static_cast<PhysicalObject&>(node));
}

void HierarchicalVisitor::visit(PhysicalObject &node)
{
    visit(static_cast<GameObjectContainer&>(node));
}

void HierarchicalVisitor::visit(PhysicalWorld &node)
{
    visit(static_cast<GameObjectContainer&>(node));
}

void HierarchicalVisitor::visit(GameObjectContainer &node)
{
    visit(static_cast<GameObject&>(node));
}

void HierarchicalVisitor::visit(GameObject &/*node*/)
{
}



}  // namespace Platformer
