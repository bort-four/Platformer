// GameObjectVisitor.cpp

#include "game_object/GameObject.h"
#include "GameObjectVisitor.h"


namespace Platformer
{


struct GameObjectVisitor::Impl
{
    Impl()
    {
    }
};



GameObjectVisitor::GameObjectVisitor()
    : _pimpl(new Impl())
{
	// ...
}

GameObjectVisitor::GameObjectVisitor(GameObjectVisitor&& /*other*/) = default;
GameObjectVisitor& GameObjectVisitor::operator=(GameObjectVisitor&& /*other*/) = default;
GameObjectVisitor::~GameObjectVisitor() = default;


void GameObjectVisitor::doProcess(GameObject &node)
{
    node.accept(*this);
}


}  // namespace Platformer

