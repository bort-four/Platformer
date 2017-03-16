// GameObject.h

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <memory>
#include <string>

#include "geometry/Point.h"
#include "geometry/Rectangle.h"
#include "Types.h"


namespace Platformer
{

class GameObjectContainer;


class GameObject
{
public:
    GameObject(const std::string &name = "[GameObject]");
    GameObject(GameObject&& other);
    virtual GameObject& operator=(GameObject&& other);
    virtual ~GameObject();

    std::string getName() const;
    SimpleGameObjectPointer getParentPointer();
    ConstSimpleGameObjectPointer getParentPointer() const;
    virtual GameObjectIteratorPtr getSubObjects() const;
    virtual Point getPosition() const;
    virtual Point mapToGlobal(const Point &point,
                              SimpleGameObjectPointer parentPtr = nullptr) const;
    virtual Rectangle mapToGlobal(const Rectangle &rect,
                                  SimpleGameObjectPointer parentPtr = nullptr) const;

    void setName(const std::string &name);
    virtual void accept(GameObjectVisitor& visitor);

private:
    friend GameObjectContainer;
    void setParentPointer(const SimpleGameObjectPointer &parentPtr);

    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};




template <TreeIteratorOrder order = Preorder>
GameObjectIteratorPtr createTreeIterator(GameObjectPointer rootPtr)
{
    return GameObjectIteratorPtr(
                new TreeIterator<GameObjectPointer, order>
                (rootPtr, [](GameObjectPointer objPtr)
                {
                    return objPtr->getSubObjects();
                }));
}



}  // namespace Platformer

#endif  // GAMEOBJECT_H
