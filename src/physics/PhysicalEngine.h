// PhysicalEngine.h

#ifndef PHYSICALENGINE_H
#define PHYSICALENGINE_H

#include <memory>

#include "Types.h"


namespace Platformer
{


class PhysicalEngine
{
public:
    PhysicalEngine();
    PhysicalEngine(PhysicalEngine&& other);
    virtual PhysicalEngine& operator=(PhysicalEngine&& other);
    virtual ~PhysicalEngine();

    PhysicalWorldPointer getWorldPtr() const;
    double getG() const;
    double getAirFrictionFactor() const;
    double getMinSpeed() const;
    double getMaxSpeed() const;
    double getMinDistance() const;
    size_t getStaticFrameCount() const;

    static double getDefaultFirictionFactor();
    static double getDefaultEnergyLostFactor();

    void setWorldPtr(PhysicalWorldPointer worldPtr);
    void updateMetadata();
    void processWorld();
    void setG(double G);
    void setAirFrictionFactor(double factor);
    void setMinSpeed(double speed);
    void setMaxSpeed(double speed);
    void setMinDistance(double distance);
    void setStaticFrameCount(size_t count);

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // PHYSICALENGINE_H
