// GameObjectContainer.h

#ifndef GAMEOBJECTCONTAINER_H
#define GAMEOBJECTCONTAINER_H

#include "GameObject.h"


namespace Platformer
{

class GameObjectContainer : public GameObject
{
public:
    GameObjectContainer(const std::string &name = "[GameObjectContainer]");
    GameObjectContainer(GameObjectContainer&& other);
    virtual GameObjectContainer& operator=(GameObjectContainer&& other);
    virtual ~GameObjectContainer();

    virtual GameObjectIteratorPtr getSubObjects() const override;
    bool isSubObject(GameObjectPointer subObjPtr) const;

    void addSubObject(GameObjectPointer subObjPtr);
    void removeSubObject(GameObjectPointer subObjPtr);
    void clearSubObjects();
    virtual void accept(GameObjectVisitor& visitor) override;

protected:
    virtual void doAddSubObject(GameObjectPointer subObjPtr);
    virtual void doRemoveSubObject(GameObjectPointer subObjPtr);

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;

};


}  // namespace Platformer

#endif  // GAMEOBJECTCONTAINER_H
