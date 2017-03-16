// QtPlatformManager.h

#ifndef QTPLATFORMMANAGER_H
#define QTPLATFORMMANAGER_H

#include <memory>
#include <string>

#include "platform/PlatformManager.h"
#include "Types.h"


namespace Platformer
{


class QtPlatformManager : public PlatformManager
{
public:
    QtPlatformManager(int &argc, char *argv[]);
    QtPlatformManager(QtPlatformManager&& other);
    virtual QtPlatformManager& operator=(QtPlatformManager&& other);
    virtual ~QtPlatformManager();

    virtual void startFrameLoop() override;
    virtual void stopFrameLoop() override;
    virtual int runMainLoop() override;

protected:
    virtual void doSetFPS(double fps) override;
    virtual void doShowWarning(const std::string &what) override;
    virtual void doShowError(const std::string &what) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // PLATFORMMANAGER_H
