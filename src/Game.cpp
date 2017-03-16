// Game.cpp

#include <iostream>

#include "platform/Platform.h"
#include "platform/PlatformManager.h"
#include "platform/Key.h"
#include "physics/TestObject.h"
#include "physics/PhysicalWorld.h"
#include "physics/PhysicalEngine.h"
#include "physics/MapPlatform.h"
#include "visitor/GamePainter.h"
#include "visualizer/Visualizer.h"
#include "Game.h"


namespace Platformer
{


struct Game::Impl
{
    Impl()
    {
    }

    void createDemoscene();
    void addPlatform(const Rectangle &rect);
    void addObject(const Rectangle &rect);

    KeyPointer _rightKeyPtr, _leftKeyPtr, _upKeyPtr, _downKeyPtr;
    PhysicalWorldPointer _worldPtr;
    GamePainterPointer _painterPtr;
    GameObjectIteratorPtr _worldIteratorPtr;
    PhysicalEnginePointer _enginePtr;
    TestObjectPointer _playerPtr;
};


/* Common TODO:
 * [OK]  encapsulate PlatformManager class
 * [OK]  encapsulate special properties of physical objects
 * optimize physical engine
*/



Game::Game()
    : _pimpl(new Impl())
{
    // create keys
    _pimpl->_rightKeyPtr.reset( new Key('d', Key::Right));
    _pimpl->_leftKeyPtr.reset(  new Key('a', Key::Left));
    _pimpl->_upKeyPtr.reset(    new Key('w', Key::Up));
    _pimpl->_downKeyPtr.reset(  new Key('s', Key::Down));

    // create world
    _pimpl->_painterPtr.reset(new GamePainter());
    _pimpl->_worldPtr.reset(new PhysicalWorld());

    _pimpl->createDemoscene();
    _pimpl->_worldIteratorPtr = createTreeIterator(_pimpl->_worldPtr);

    // create physical engine
    _pimpl->_enginePtr.reset(new PhysicalEngine());
    _pimpl->_enginePtr->setWorldPtr(_pimpl->_worldPtr);
    _pimpl->_worldPtr->setEnginePtr(_pimpl->_enginePtr);

    // frame handler
    Platform::instance()->frameHandler = [this]()
    {
        int dirH = (_pimpl->_rightKeyPtr->isPressed() ? 1 : 0) + (_pimpl->_leftKeyPtr->isPressed() ? -1 : 0);
        int dirV = 0; //(_pimpl->_upKeyPtr->isPressed() ?   -1 : 0) + (_pimpl->_downKeyPtr->isPressed() ?  1 : 0);

        Point dir(dirH, dirV);

        static const double ACCELERATION = 1000;

        _pimpl->_playerPtr->setSpeed(_pimpl->_playerPtr->getSpeed()
                                     + dir * ACCELERATION * Platform::instance()->getActualFrameTime());
        _pimpl->_enginePtr->processWorld();
        _pimpl->_painterPtr->visit(_pimpl->_worldIteratorPtr);
        Platform::visualizer()->refresh();
    };

    _pimpl->_upKeyPtr->onPress = [this]()
    {
//        if (_pimpl->_playerPtr->getContiguousObject(Down) != nullptr)
            _pimpl->_playerPtr->setSpeed(_pimpl->_playerPtr->getSpeed() + Point(0, -780));
    };

    // start
    Platform::instance()->setFPS(60);
    Platform::instance()->startFrameLoop();
}

Game::Game(Game&& /*other*/) = default;
Game& Game::operator=(Game&& /*other*/) = default;
Game::~Game() = default;


void Game::Impl::createDemoscene()
{
    static const int H = 20;
    const int h = 36;

    // scene borders
    Rectangle rect(H, H, h * 16, h * 11);

    Point sizeH(rect.getWidth(), H);
    Point sizeV(H, rect.getHeight());
    addPlatform(Rectangle(Point(0, -H), sizeH));
    addPlatform(Rectangle(Point(0, rect.getHeight()), sizeH));
    addPlatform(Rectangle(Point(-H, 0), sizeV));
    addPlatform(Rectangle(Point(rect.getWidth(), 0), sizeV));

    // map
    static const double eps = -10;
    addPlatform(Rectangle(0  * h, 6  * h, 3 * h, 1 * h));
    addPlatform(Rectangle(7  * h, 5  * h, 4 * h, 1 * h));

    addPlatform(Rectangle(1  * h, 10 * h, 6 * h, 1 * h));
    addPlatform(Rectangle(13 * h, 8  * h, 3 * h, 1 * h));

    addObject(Rectangle(4  * h, 9  * h + eps,     2   * h, 1 * h));
    addObject(Rectangle(7  * h, 1  * h + eps * 3, 3   * h, 1 * h));
    addObject(Rectangle(9  * h, 2  * h + eps * 2, 2   * h, 2 * h));
    addObject(Rectangle(8  * h, 4  * h + eps,     2.5 * h, 1 * h));
    addObject(Rectangle(10 * h, 9  * h + eps,     2   * h, 2 * h));

    _playerPtr.reset(new TestObject());
    _playerPtr->setPosition(Point(14 * h, 3 * h + eps));
    _playerPtr->setSize(Point(1 * h, 2 * h));
    _worldPtr->addSubObject(_playerPtr);
}


void Game::Impl::addPlatform(const Rectangle &rect)
{
    _worldPtr->addSubObject(std::make_shared<MapPlatform>(rect));
}


void Game::Impl::addObject(const Rectangle &rect)
{
    TestObjectPointer objPtr(new TestObject());
    objPtr->setPosition(rect.getPosition());
    objPtr->setSize(rect.getSize());

    _worldPtr->addSubObject(objPtr);
}



}  // namespace Platformer
