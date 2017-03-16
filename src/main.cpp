// main.cpp

#include <iostream>
#include <vector>

#include <QApplication>

#include "platform/Platform.h"
#include "platform/qt5/QtPlatformManager.h"
#include "game_object/GameObject.h"
#include "game_object/GameObjectContainer.h"
#include "physics/PhysicalWorld.h"
#include "physics/TestObject.h"
#include "visitor/SimpleVisitor.h"
#include "visitor/GamePainter.h"
#include "visitor/SimpleHierarchicalVisitor.h"
#include "Iterator.h"
#include "Game.h"

using namespace Platformer;


int main(int argc, char *argv[])
try
{
    PlatformManagerPointer managerPtr(new QtPlatformManager(argc, argv));
    Platform::instance()->initialize(managerPtr);
    Game game;

    return Platform::instance()->runMainLoop();
}
catch (const std::exception &error)
{
    std::cerr << error.what() << std::endl;
}




/*
GameObjectContainerPointer c1(new GameObjectContainer("c1"));
GameObjectContainerPointer c2(new GameObjectContainer("c2"));
GameObjectContainerPointer c3(new GameObjectContainer("c3"));
GameObjectPointer o1(new TestObject("o1"));
GameObjectPointer o2(new TestObject("o2"));
GameObjectPointer o3(new TestObject("o3"));
c1->addSubObject(c2);
c1->addSubObject(c3);
c2->addSubObject(o1);
c2->addSubObject(o2);
c3->addSubObject(o3);


SimpleVisitor<GameObject> visitor([](GameObject& node)
{
    std::cout << node.getName() << std::endl;
});
visitor.visit(createTreeIterator(c1));

std::cout << "==============" << std::endl;
forEach(createTreeIterator(c1), [](GameObjectPointer objPtr)
{
    for (SimpleGameObjectPointer parentPtr = objPtr->getParentPointer();
         parentPtr != nullptr; parentPtr = parentPtr->getParentPointer())
        std::cout << "  ";

    std::cout << objPtr->getName() << std::endl;
});
std::cout << "==============" << std::endl;
*/
