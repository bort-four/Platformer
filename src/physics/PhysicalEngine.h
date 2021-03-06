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
    double getGravityAcceleration() const;
    double getAirFrictionDeceleration() const;
    double getMaxSpeed() const;

    static double getDefaultFirictionFactor();
    static double getDefaultHitRecoveryFactor();

    void updateMetadata();
    void processWorld();
    void setWorldPtr(PhysicalWorldPointer worldPtr);
    void setGravityAcceleration(double gravityAcceleration);
    void setAirFrictionDeceleration(double factor);
    void setMaxSpeed(double speed);

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // PHYSICALENGINE_H
