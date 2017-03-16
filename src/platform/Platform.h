// Platform.h

#ifndef PLATFORM_H
#define PLATFORM_H

#include <memory>

#include "Types.h"
#include "Key.h"


namespace Platformer
{


class Platform
{
public:
    using FrameHandler = std::function<void()>;
    virtual ~Platform();

    static Platform *instance();
    static VisualizerPointer visualizer();
    double getFPS() const;
    double getActualFrameTime() const;
    VisualizerPointer getVisualizer() const;

    void initialize(PlatformManagerPointer managerPtr);
    void setFPS(double fps);
    void showWarning(const std::string &what);
    void showError(const std::string &what);
    void triggerKey(char ch, bool isPressed);
    void triggerKey(Key::KeyId id, bool isPressed);
    void startFrameLoop();
    void stopFrameLoop();
    int  runMainLoop();
    void registerKey(SimpleKeyPointer keyPtr, char ch);
    void registerKey(SimpleKeyPointer keyPtr, Key::KeyId id);
    void unregisterKey(SimpleKeyPointer keyPtr);

    FrameHandler frameHandler;

//    PlatformManagerPointer getPlatformManager() const;
//    static PlatformManagerPointer manager();

protected:
    Platform();

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // PLATFORM_H
