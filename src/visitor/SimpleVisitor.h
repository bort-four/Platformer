// SimpleVisitor.h

#ifndef SIMPLEVISITOR_H
#define SIMPLEVISITOR_H

#include "GameObjectVisitor.h"


namespace Platformer
{

template <class PrecessedNodeType>
class SimpleVisitor : public GameObjectVisitor
{
public:
    using NodeHandler = std::function<void(PrecessedNodeType&)>;

    SimpleVisitor(NodeHandler handler);
    SimpleVisitor(SimpleVisitor&& other) = default;
    virtual SimpleVisitor& operator=(SimpleVisitor&& other) = default;
    virtual ~SimpleVisitor() = default;

    using GameObjectVisitor::visit;
    void visit(PrecessedNodeType& nodePtr);

private:
    NodeHandler _handler;
};



template <class PrecessedNodeType>
SimpleVisitor<PrecessedNodeType>::SimpleVisitor(SimpleVisitor::NodeHandler handler)
    : GameObjectVisitor()
    , _handler(handler)
{
    if (_handler == nullptr)
        throw std::logic_error("SimpleVisitor: node handler is null");
}


template<class PrecessedNodeType>
void SimpleVisitor<PrecessedNodeType>::visit(PrecessedNodeType &nodePtr)
{
    _handler(nodePtr);
}


//template<class PrecessedNodeType>
//void SimpleVisitor<PrecessedNodeType>::doPreprocess(GameObject&)
//{
//    std::cout << "{" << std::endl;
//}


//template<class PrecessedNodeType>
//void SimpleVisitor<PrecessedNodeType>::doPostprocess(GameObject&)
//{
//    std::cout << "}" << std::endl;
//}


}  // namespace Platformer

#endif  // SIMPLEVISITOR_H
