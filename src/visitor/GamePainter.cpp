// GamePainter.cpp

#include "platform/Platform.h"
#include "visualizer/Visualizer.h"
#include "physics/TestObject.h"
#include "GamePainter.h"


namespace Platformer
{


struct GamePainter::Impl
{
    Impl()
    {
    }
};



GamePainter::GamePainter()
    : _pimpl(new Impl())
{
}


GamePainter::GamePainter(GamePainter&& /*other*/) = default;
GamePainter& GamePainter::operator=(GamePainter&& /*other*/) = default;
GamePainter::~GamePainter() = default;


void GamePainter::visit(PhysicalObject &node)
{
    forEach(node.getGeometry(), [&node](const Rectangle &rect)
    {
        Platform::visualizer()->drawRect(node.mapToGlobal(rect),
                                         node.isMovable(),
                                         false/*node.isStatic()*/,
                                         false/*node.isStand()*/);
    });
}


void GamePainter::doPreprocessAll()
{
    Platform::visualizer()->clear();
}


//void GamePainter::visit(TestObject &node)
//{
//    forEach(node.getGeometry(), [&node](const Rectangle &rect)
//    {
//        Platform::visualizer()->drawRect(node.mapToGlobal(rect));
//    });
//}


//void Platformer::GamePainter::visit(PhysicalWorld &)
//{
//    Platform::visualizer()->clear();
//}


}  // namespace Platformer




