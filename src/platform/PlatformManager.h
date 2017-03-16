// PlatformManager.h

#ifndef PLATFORMMANAGER_H
#define PLATFORMMANAGER_H

#include <memory>
#include <string>

#include "Types.h"
#include "Key.h"


namespace Platformer
{


class PlatformManager
{
public:
    using FrameHandler = std::function<void()>;

    PlatformManager(PlatformManager&& other);
    virtual PlatformManager& operator=(PlatformManager&& other);
    virtual ~PlatformManager();

    double getFPS() const;
    double getActualFrameTime() const;
    VisualizerPointer getVisualizer() const;

    void setFPS(double fps);
    void setVisualizer(VisualizerPointer visualizerPtr);
    void showWarning(const std::string &what);
    void showError(const std::string &what);
    void triggerKey(char ch, bool isPressed);
    void triggerKey(Key::KeyId id, bool isPressed);
    virtual void startFrameLoop() = 0;
    virtual void stopFrameLoop() = 0;
    virtual int runMainLoop() = 0;
    void registerKey(SimpleKeyPointer keyPtr, char ch);
    void registerKey(SimpleKeyPointer keyPtr, Key::KeyId id);
    void unregisterKey(SimpleKeyPointer keyPtr);

    FrameHandler frameHandler;

protected:
    PlatformManager(int &argc, char *argv[]);
    void updateActualFrameTime();
    virtual void doSetFPS(double fps);
    virtual void doSetVisualizer(VisualizerPointer visualizerPtr);
    virtual void doShowWarning(const std::string &what);
    virtual void doShowError(const std::string &what);

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // PLATFORMMANAGER_H
