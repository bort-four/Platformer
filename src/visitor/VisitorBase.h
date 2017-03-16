// VisitorBase.h

#ifndef VISITORBASE_H
#define VISITORBASE_H

#include <memory>

#include "Types.h"
#include "Iterator.h"


namespace Platformer
{

template <class BaseNodeType>
class VisitorBase
{
public:
    using PointerType = Pointer<BaseNodeType>;
    using IteratorType = IteratorPointer<PointerType>;

    VisitorBase() {}
    VisitorBase(VisitorBase&& other) = default;
    virtual VisitorBase& operator=(VisitorBase&& other) = default;
    virtual ~VisitorBase() = default;

    void visit(IteratorType iterPtr);

protected:
    virtual void doPreprocess(BaseNodeType& /*node*/) {}
    virtual void doProcess(BaseNodeType& /*node*/) = 0;
    virtual void doPostprocess(BaseNodeType& /*node*/) {}
    virtual void doPreprocessAll() {}
    virtual void doPostprocessAll() {}
};


template <class BaseNodeType>
void VisitorBase<BaseNodeType>::visit(VisitorBase<BaseNodeType>::IteratorType iterPtr)
{
    doPreprocessAll();

    for (iterPtr->first(); !iterPtr->isDone(); iterPtr->next())
    {
        doPreprocess(*iterPtr->current());
        doProcess(*iterPtr->current());
        doPostprocess(*iterPtr->current());
    }

    doPostprocessAll();
}


}  // namespace Platformer

#endif  // VISITORBASE_H
