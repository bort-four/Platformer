// SimpleHierarchicalVisitor.h

#ifndef SIMPLEHIERARCHICALVISITOR_H
#define SIMPLEHIERARCHICALVISITOR_H

#include <memory>

#include "HierarchicalVisitor.h"


namespace Platformer
{



template <class PrecessedNodeType>
class SimpleHierarchicalVisitor : public HierarchicalVisitor
{
public:
    using NodeHandler = std::function<void(PrecessedNodeType&)>;

    SimpleHierarchicalVisitor(NodeHandler handler);
    SimpleHierarchicalVisitor(SimpleHierarchicalVisitor&& other) = default;
    virtual SimpleHierarchicalVisitor& operator=(SimpleHierarchicalVisitor&& other) = default;
    virtual ~SimpleHierarchicalVisitor() = default;

    using HierarchicalVisitor::visit;
    void visit(PrecessedNodeType& nodePtr);

private:
    NodeHandler _handler;
};




template <class PrecessedNodeType>
SimpleHierarchicalVisitor<PrecessedNodeType>::SimpleHierarchicalVisitor(SimpleHierarchicalVisitor::NodeHandler handler)
    : HierarchicalVisitor()
    , _handler(handler)
{
    if (_handler == nullptr)
        throw std::logic_error("SimpleVisitor: node handler is null");
}


template<class PrecessedNodeType>
void SimpleHierarchicalVisitor<PrecessedNodeType>::visit(PrecessedNodeType &nodePtr)
{
    _handler(nodePtr);
}


}  // namespace Platformer

#endif  // SIMPLEHIERARCHICALVISITOR_H
