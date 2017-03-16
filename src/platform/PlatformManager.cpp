// PlatformManager.cpp

#include <iostream>
#include <map>
#include <set>
#include <chrono>

//#include <QApplication>
//#include <QMessageBox>
//#include <QTimer>

//#include "PlatformerApplication.h"
#include "PlatformManager.h"


namespace Platformer
{

struct PlatformManager::Impl
{
    using KeySet = std::set<SimpleKeyPointer>;

    double _fps = 60;
    VisualizerPointer _visualizerPtr;

    std::map<char, KeySet> _charKeyRegistry;
    std::map<Key::KeyId, KeySet> _idKeyRegistry;

    std::chrono::high_resolution_clock::time_point _lastTimePoint;
    std::chrono::duration<double> _frameTime;
};


PlatformManager::PlatformManager(int &/*argc*/, char */*argv*/[])
    : _pimpl(new Impl())
{
}


PlatformManager::PlatformManager(PlatformManager&& other) = default;
PlatformManager& PlatformManager::operator=(PlatformManager&& other) = default;

PlatformManager::~PlatformManager()
{
}


double PlatformManager::getFPS() const
{
    return _pimpl->_fps;
}

double PlatformManager::getActualFrameTime() const
{
    return _pimpl->_frameTime.count();
}

VisualizerPointer PlatformManager::getVisualizer() const
{
    return _pimpl->_visualizerPtr;
}


void PlatformManager::setFPS(double fps)
{
    _pimpl->_fps = fps;
    doSetFPS(fps);
}


void PlatformManager::setVisualizer(VisualizerPointer visualizerPtr)
{
    _pimpl->_visualizerPtr = visualizerPtr;
}


void PlatformManager::showWarning(const std::string &what)
{
    std::cerr << what << std::endl;
    doShowWarning(what);
}

void PlatformManager::showError(const std::string &what)
{
    std::cerr << what << std::endl;
    doShowError(what);
}


void PlatformManager::registerKey(SimpleKeyPointer keyPtr, char ch)
{
    _pimpl->_charKeyRegistry[ch].insert(keyPtr);
}


void PlatformManager::registerKey(SimpleKeyPointer keyPtr, Key::KeyId id)
{
    _pimpl->_idKeyRegistry[id].insert(keyPtr);
}


void PlatformManager::unregisterKey(SimpleKeyPointer keyPtr)
{
    forEach(keyPtr->getCharIterator(), [this, keyPtr](char ch)
    {
        _pimpl->_charKeyRegistry[ch].erase(keyPtr);
    });

    forEach(keyPtr->getKeyIdIterator(), [this, keyPtr](Key::KeyId id)
    {
        _pimpl->_idKeyRegistry[id].erase(keyPtr);
    });
}


void PlatformManager::updateActualFrameTime()
{
    std::chrono::high_resolution_clock::time_point point
            = std::chrono::high_resolution_clock::now();
    _pimpl->_frameTime = point - _pimpl->_lastTimePoint;
    _pimpl->_lastTimePoint = point;
}


void PlatformManager::doSetFPS(double /*fps*/)
{
}

void PlatformManager::doSetVisualizer(VisualizerPointer /*visualizerPtr*/)
{
}

void PlatformManager::doShowWarning(const std::string &/*what*/)
{
}

void PlatformManager::doShowError(const std::string &/*what*/)
{
}

void PlatformManager::triggerKey(char ch, bool isPressed)
{
    // TODO: do not process char that has no registred Key objects

    for (SimpleKeyPointer keyPtr : _pimpl->_charKeyRegistry[ch])
        keyPtr->setIsPressed(isPressed);
}

void PlatformManager::triggerKey(Key::KeyId id, bool isPressed)
{
    for (SimpleKeyPointer keyPtr : _pimpl->_idKeyRegistry[id])
        keyPtr->setIsPressed(isPressed);
}



/*
_pimpl->_frameTimerPtr.reset(new QTimer());
QObject::connect(_pimpl->_frameTimerPtr.get(), &QTimer::timeout, [this]()
{
    if (frameHandler != nullptr)
        frameHandler();
});


void PlatformManager::initialize(int argc, char *argv[])
{
    _pimpl->_applicationPtr.reset(new PlatformerApplication(argc, argv));
}


void PlatformManager::setFPS(double fps)
{
    _pimpl->_fps = fps;
    _pimpl->_frameTimerPtr->setInterval(static_cast<int>(1000 / _pimpl->_fps));
}

void PlatformManager::startFrameLoop()
{
    _pimpl->_frameTimerPtr->start();
}

void PlatformManager::stopFrameLoop()
{
    _pimpl->_frameTimerPtr->stop();
}


PlatformManager *PlatformManager::instance()
{
    static std::unique_ptr<PlatformManager> instancePtr;

    if (instancePtr == nullptr)
        instancePtr.reset(new PlatformManager());

    return instancePtr.get();
}


int PlatformManager::runMainLoop()
{
    return _pimpl->_applicationPtr->exec();
}


void PlatformManager::showWarning(const std::string &what)
{
    QMessageBox::warning(nullptr, QObject::tr("Warning"), what.c_str());
}

void PlatformManager::showError(const std::string &what)
{
    QMessageBox::critical(nullptr, QObject::tr("Error"), what.c_str());
}
*/


}  // namespace Platformer
