// PhysicalEngine.cpp

#include <vector>
#include <cmath>

#include <QDebug>

#include "platform/Platform.h"
#include "visualizer/Visualizer.h"
#include "game_object/GameObject.h"
#include "game_object/GameObjectContainer.h"
#include "visitor/SimpleHierarchicalVisitor.h"

#include "PhysicalWorld.h"
#include "PhysicalObject.h"
#include "PhysicalEngine.h"
#include "StrictCollisionProcessor.h"


namespace Platformer
{



struct PhysicalEngine::Impl
{
    void applyPhisicalRules(double frameTimeSec);
    void applyFrictionBetween(SimplePhysicalObjectPointer firstObjectPtr,
                              SimplePhysicalObjectPointer secondObjectPtr,
                              Direction connectionDir, double frameTimeSec);

    inline double sign(double value)
    {
        return value < 0 ? -1 : 1;
    }

    PhysicalWorldPointer _worldPtr;
    std::vector<SimplePhysicalObjectPointer> _objectVect;
    SimpleHierarchicalVisitorPointer<PhysicalObject> _objectCollectorPtr;
    CollisionProcessorPointer _collisionProcessor;

    double _gravityAcceleration = 2000.0;
    double _airFrictionDeceleration = 50;
    double _maxSpeed = 5000;
};


PhysicalEngine::PhysicalEngine()
    : _pimpl(new Impl())
{
    _pimpl->_objectCollectorPtr.reset(new SimpleHierarchicalVisitor<PhysicalObject>
                                      ([this](PhysicalObject &object)
    {
        _pimpl->_objectVect.emplace_back(&object);
    }));

    _pimpl->_collisionProcessor = std::make_shared<StrictCollisionProcessor>(this);
}


PhysicalEngine::PhysicalEngine(PhysicalEngine&& /*other*/) = default;
PhysicalEngine& PhysicalEngine::operator=(PhysicalEngine&& /*other*/) = default;
PhysicalEngine::~PhysicalEngine() = default;


void PhysicalEngine::updateMetadata()
{
    if (getWorldPtr() == nullptr)
        throw std::logic_error("PhysicalEngine::updateObjectCache: world is not set");

    _pimpl->_objectVect.clear();
    _pimpl->_objectCollectorPtr->visit(getWorldPtr()->getSubObjects());

    // TODO: CollisionProcessor::updateMetadata
    _pimpl->_collisionProcessor->updateMetadata();
}


void PhysicalEngine::processWorld()
{
    // get frame time
    double frameTimeSec = Platform::instance()->getActualFrameTime();

    // calculate objects speeds by physical rules
    _pimpl->applyPhisicalRules(frameTimeSec);

    // move objects & process collisions
    _pimpl->_collisionProcessor->processFrame(frameTimeSec);
}


void PhysicalEngine::Impl::applyPhisicalRules(double frameTimeSec)
{
    for (SimplePhysicalObjectPointer objectPtr : _objectVect)
    {
        if (objectPtr->isMovable())
        {
            // apply gravity
            objectPtr->setSpeed(objectPtr->getSpeed() + Point(0, _gravityAcceleration * frameTimeSec));

            // apply air friction
            double frictionValue = _airFrictionDeceleration * frameTimeSec;
            double frictionAngle = std::atan2(-objectPtr->getSpeed().getY(),
                                              -objectPtr->getSpeed().getX());
            Point frictionVect(frictionValue * std::cos(frictionAngle),
                               frictionValue * std::sin(frictionAngle));

            if (objectPtr->getSpeed().getLength() < frictionVect.getLength())
                objectPtr->setSpeed(0);
            else
                objectPtr->setSpeed(objectPtr->getSpeed() + frictionVect);
        }

        // apply friction with another objects
        SimplePhysicalObjectPointer downObjectPtr  = objectPtr->getContiguousObject(Down);
        SimplePhysicalObjectPointer rightObjectPtr = objectPtr->getContiguousObject(Right);

        applyFrictionBetween(objectPtr, downObjectPtr,  Down,  frameTimeSec);
        applyFrictionBetween(objectPtr, rightObjectPtr, Right, frameTimeSec);
    }
}


void PhysicalEngine::Impl::applyFrictionBetween(SimplePhysicalObjectPointer firstObjectPtr,
                                                SimplePhysicalObjectPointer secondObjectPtr,
                                                Direction connectionDir,
                                                double frameTimeSec)
{
    if (firstObjectPtr == nullptr || secondObjectPtr == nullptr)
        return;

    const double frictionFactor = (firstObjectPtr->getFrictionFactor()
                             + secondObjectPtr->getFrictionFactor()) / 2;
    const double frictionValue = frictionFactor * frameTimeSec;
    const bool isHorizontalFriction = !(connectionDir == Right || connectionDir == Left);

    double firstObjectSpeed = firstObjectPtr->getSpeed().getProjection(isHorizontalFriction);
    double secondObjectSpeed = secondObjectPtr->getSpeed().getProjection(isHorizontalFriction);

    if (firstObjectPtr->isMovable())
    {
        double firstRelativeSpeed  = firstObjectSpeed - secondObjectSpeed;
        Point firstSpeedVect  = firstObjectPtr->getSpeed();

        if (std::abs(firstRelativeSpeed) > frictionValue)
            firstSpeedVect.setProjection(firstObjectSpeed
                                         - frictionValue * sign(firstRelativeSpeed), isHorizontalFriction);
        else
            firstSpeedVect.setProjection(secondObjectSpeed, isHorizontalFriction);

        firstObjectPtr->setSpeed(firstSpeedVect);
    }

    if (secondObjectPtr->isMovable())
    {
        double secondRelativeSpeed = secondObjectSpeed - firstObjectSpeed;
        Point secondSpeedVect = secondObjectPtr->getSpeed();

        if (std::abs(secondRelativeSpeed) > frictionValue)
            secondSpeedVect.setProjection(secondObjectSpeed
                                          - frictionValue * sign(secondRelativeSpeed), isHorizontalFriction);
        else
            secondSpeedVect.setProjection(firstObjectSpeed, isHorizontalFriction);

        secondObjectPtr->setSpeed(secondSpeedVect);
    }
}



/*
void PhysicalEngine::Impl::applyFrictionBetween(SimplePhysicalObjectPointer obj1Ptr,
                                                SimplePhysicalObjectPointer obj2Ptr,
                                                bool isHorizontal, double frameTime)
{
    if (obj1Ptr == nullptr || obj2Ptr == nullptr)
        return;

    Point speedV1 = obj1Ptr->getSpeed();
    Point speedV2 = obj2Ptr->getSpeed();

    const double FRICTION_FACTOR
            = (obj1Ptr->getFrictionFactor() + obj2Ptr->getFrictionFactor()) / 2;

    if (isHorizontal)
    {
        int signY1 = (speedV1.getY() > 0) ? 1 : ((speedV1.getY() < 0) ? -1 : 0);
        int signY2 = (speedV2.getY() > 0) ? 1 : ((speedV2.getY() < 0) ? -1 : 0);

        if (obj1Ptr->isMovable())
            obj1Ptr->setSpeed(Point(speedV1.getX(), speedV1.getY() - signY1 * FRICTION_FACTOR * frameTime));

        if (obj2Ptr->isMovable())
            obj2Ptr->setSpeed(Point(speedV2.getX(), speedV2.getY() - signY2 * FRICTION_FACTOR * frameTime));
    }
    else
    {
        int signX1 = (speedV1.getX() > 0) ? 1 : ((speedV1.getX() < 0) ? -1 : 0);
        int signX2 = (speedV2.getX() > 0) ? 1 : ((speedV2.getX() < 0) ? -1 : 0);

        if (obj1Ptr->isMovable())
            obj1Ptr->setSpeed(Point(speedV1.getX() - signX1 * FRICTION_FACTOR * frameTime, speedV1.getY()));

        if (obj2Ptr->isMovable())
            obj2Ptr->setSpeed(Point(speedV2.getX() - signX2 * FRICTION_FACTOR * frameTime, speedV2.getY()));
    }
}
*/



PhysicalWorldPointer PhysicalEngine::getWorldPtr() const
{
    return _pimpl->_worldPtr;
}

double PhysicalEngine::getGravityAcceleration() const
{
    return _pimpl->_gravityAcceleration;
}

double PhysicalEngine::getAirFrictionDeceleration() const
{
    return _pimpl->_airFrictionDeceleration;
}

double PhysicalEngine::getMaxSpeed() const
{
    return _pimpl->_maxSpeed;
}

double PhysicalEngine::getDefaultFirictionFactor()
{
    return 100;
}

double PhysicalEngine::getDefaultHitRecoveryFactor()
{
    return 0.3;
}


void PhysicalEngine::setWorldPtr(PhysicalWorldPointer worldPtr)
{
    _pimpl->_worldPtr = worldPtr;
    updateMetadata();
}

void PhysicalEngine::setGravityAcceleration(double gravityAcceleration)
{
    _pimpl->_gravityAcceleration = gravityAcceleration;
}

void PhysicalEngine::setAirFrictionDeceleration(double factor)
{
    _pimpl->_airFrictionDeceleration = factor;
}

void PhysicalEngine::setMaxSpeed(double speed)
{
    _pimpl->_maxSpeed = speed;
}



}  // namespace Platformer



/* GARBAGE

Point PhysicalEngine::Impl::getRotatedVector(const Point &vector, double angle)
{
    double length = sqrt(vector.getX() * vector.getX() + vector.getY() * vector.getY());
    double pointAngle = atan2(vector.getY(), vector.getX());

    return Point(cos(pointAngle + angle) * length,
                 sin(pointAngle + angle) * length);
}

*/
