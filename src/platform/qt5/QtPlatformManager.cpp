// PlatformManager.cpp

#include <iostream>

#include <QApplication>
#include <QMessageBox>
#include <QTimer>

#include "QtVisualizer.h"
#include "QtApplication.h"
#include "QtPlatformManager.h"


namespace Platformer
{

struct QtPlatformManager::Impl
{
//    Impl(int &argc, char *argv[])
//        : _application(argc, argv)
//    {
//    }

//    std::unique_ptr<QtApplication> _applicationPtr;
    QtApplication *_applicationPtr;
//    std::unique_ptr<QTimer> _frameTimerPtr;
    QTimer *_frameTimerPtr;
    double _fps = 0;
};



QtPlatformManager::QtPlatformManager(int &argc, char *argv[])
    : PlatformManager(argc, argv)
    , _pimpl(new Impl(/*argc, argv*/))
{
    _pimpl->_applicationPtr = new QtApplication(argc, argv);
    _pimpl->_frameTimerPtr = new QTimer();
    setVisualizer(std::make_shared<QtVisualizer>());

    QObject::connect(_pimpl->_frameTimerPtr/*.get()*/, &QTimer::timeout, [this]()
    {
        updateActualFrameTime();

        if (frameHandler != nullptr)
            frameHandler();
    });
}


QtPlatformManager::QtPlatformManager(QtPlatformManager&& other) = default;
QtPlatformManager& QtPlatformManager::operator=(QtPlatformManager&& other) = default;

QtPlatformManager::~QtPlatformManager()
{
}


void Platformer::QtPlatformManager::startFrameLoop()
{
    _pimpl->_frameTimerPtr->start(static_cast<int>(1000.0 / getFPS()));
    updateActualFrameTime();
}

void Platformer::QtPlatformManager::stopFrameLoop()
{
    _pimpl->_frameTimerPtr->stop();
}

int Platformer::QtPlatformManager::runMainLoop()
{
    return _pimpl->_applicationPtr->exec();
}

void Platformer::QtPlatformManager::doSetFPS(double fps)
{
    _pimpl->_frameTimerPtr->setInterval(static_cast<int>(1000.0 / fps));
}

void Platformer::QtPlatformManager::doShowWarning(const std::string &what)
{
    QMessageBox::warning(nullptr, QObject::tr("Warning"), what.c_str());
}

void Platformer::QtPlatformManager::doShowError(const std::string &what)
{
    QMessageBox::critical(nullptr, QObject::tr("Error"), what.c_str());
}


}  // namespace Platformer
