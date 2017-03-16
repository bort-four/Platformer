// Platform.cpp

#include "Platform.h"
#include "PlatformManager.h"


namespace Platformer
{


struct Platform::Impl
{
    Impl()
    {
    }

    inline void check()
    {
        if (_managerPtr == nullptr)
            throw std::logic_error(
                    "Platform::getPlatformManager: platform manager is not set");
    }

    PlatformManagerPointer _managerPtr;
};


Platform::Platform()
    : _pimpl(new Impl())
{
}


Platform::~Platform()
{
}


Platform *Platform::instance()
{
    static std::unique_ptr<Platform> instancePtr;

    if (instancePtr == nullptr)
        instancePtr.reset(new Platform());

    return instancePtr.get();
}

VisualizerPointer Platform::visualizer()
{
    return instance()->getVisualizer();
}

double Platform::getFPS() const
{
    _pimpl->check();
    return _pimpl->_managerPtr->getFPS();
}

double Platform::getActualFrameTime() const
{
    _pimpl->check();
    return _pimpl->_managerPtr->getActualFrameTime();
}

VisualizerPointer Platform::getVisualizer() const
{
    _pimpl->check();
    return _pimpl->_managerPtr->getVisualizer();
}




void Platform::initialize(PlatformManagerPointer managerPtr)
{
    _pimpl->_managerPtr = managerPtr;

    _pimpl->_managerPtr->frameHandler = [this]()
    {
        if (frameHandler != nullptr)
            frameHandler();
    };
}


void Platform::setFPS(double fps)
{
    _pimpl->check();
    _pimpl->_managerPtr->setFPS(fps);
}


void Platform::showWarning(const std::string &what)
{
    _pimpl->check();
    _pimpl->_managerPtr->showWarning(what);
}

void Platform::showError(const std::string &what)
{
    _pimpl->check();
    _pimpl->_managerPtr->showError(what);
}

void Platform::triggerKey(char ch, bool isPressed)
{
    _pimpl->check();
    _pimpl->_managerPtr->triggerKey(ch, isPressed);
}

void Platform::triggerKey(Key::KeyId id, bool isPressed)
{
    _pimpl->check();
    _pimpl->_managerPtr->triggerKey(id, isPressed);
}

void Platform::startFrameLoop()
{
    _pimpl->check();
    _pimpl->_managerPtr->startFrameLoop();
}

void Platform::stopFrameLoop()
{
    _pimpl->check();
    _pimpl->_managerPtr->stopFrameLoop();
}

int Platform::runMainLoop()
{
    _pimpl->check();
    return _pimpl->_managerPtr->runMainLoop();
}

void Platform::registerKey(SimpleKeyPointer keyPtr, char ch)
{
    _pimpl->check();
    _pimpl->_managerPtr->registerKey(keyPtr, ch);
}

void Platform::registerKey(SimpleKeyPointer keyPtr, Key::KeyId id)
{
    _pimpl->check();
    _pimpl->_managerPtr->registerKey(keyPtr, id);
}

void Platform::unregisterKey(SimpleKeyPointer keyPtr)
{
    _pimpl->check();
    _pimpl->_managerPtr->unregisterKey(keyPtr);
}



//PlatformManagerPointer Platform::getPlatformManager() const
//{
//    if (_pimpl->_managerPtr == nullptr)
//        throw std::logic_error("Platform::getPlatformManager: platform manager is not set");

//    return _pimpl->_managerPtr;
//}

//PlatformManagerPointer Platform::manager()
//{
//    return instance()->getPlatformManager();
//}

}  // namespace Platformer
