// StrictCollisionProcessor.cpp

#include <assert.h>
#include <vector>

#include "Iterator.h"
#include "geometry/Point.h"
#include "PhysicalObject.h"
#include "PhysicalWorld.h"
#include "PhysicalEngine.h"
#include "visitor/SimpleHierarchicalVisitor.h"
#include "StrictCollisionProcessor.h"


namespace Platformer
{


struct ObjectMetadata
{
    ObjectMetadata(SimplePhysicalObjectPointer objectPtr)
        : _objectPtr(objectPtr)
    {
    }

    SimplePhysicalObjectPointer _objectPtr = nullptr;
    size_t _lastConnectionNum = 0;
    Point _lastPosition;
};


struct CollisionInfo
{
    size_t _lessObjectNum = 0;
    size_t _greaterObectNum = 0;
    double _timeRate = 1;
    bool _hasCollision = false;
    Direction _direction = Right;
};



struct StrictCollisionProcessor::Impl
{
    // processing steps
    void doPreProcess(double frameTimeSec);
    void processCollisions(double fullframeTimeSec);
    void doPostProcess(double frameTimeSec);

    // procedures
    void processCollision(const CollisionInfo &collision, double frameTimeSec);
    void activate(ObjectMetadata *metadataPtr,
                  ObjectMetadata *parentMetadataPtr);
    void processStand(ObjectMetadata *metadataPtr);

    // functions
    CollisionInfo findCollisionBetween(size_t lessObjectNum, size_t greaterObjectNum, double frameTimeSec);

    void solvePlatformHit(double objectSpeed, double PlatformSpeed,
                          double speedRecoveryFactor,
                          double &newObjectSpeed) const;

    void solveCentralHit(double firstObjectSpeed,       double firstObjectMass,
                         double secondObjectSpeed,      double secondObjectMass,
                         double speedRecoveryFactor,
                         double &newFirstObjectSpeed,   double &newSecondObjectSpeed) const;


    inline double abs(double value)
    {
        return value < 0 ? -value : value;
    }

    inline bool doubleCompareEqual(double firstValue, double secondValue)
    {
        return abs(firstValue - secondValue) < DOUBLE_COMPARE_ERROR;
    }

    inline bool doubleCompareLess(double firstValue, double secondValue)
    {
        return firstValue < secondValue + DOUBLE_COMPARE_ERROR;
    }


    // data members
    PhysicalWorldPointer _worldPtr;
    SimpleHierarchicalVisitorPointer<PhysicalObject> _objectCollectorPtr;
    std::vector<ObjectMetadata> _objectVect;
    size_t _totalConnectionCount;

    // constants
    const double ABSOLUTE_TIME_ERROR  = 0.0001;
    const double DOUBLE_COMPARE_ERROR = 0.0001;
    // const size_t STATIC_FRAME_COUNT   = 50;
};




StrictCollisionProcessor::StrictCollisionProcessor(SimplePhysicalEnginePointer enginePtr)
    : CollisionProcessor(enginePtr)
    , _pimpl(new Impl())
{
    _pimpl->_worldPtr = getEnginePtr()->getWorldPtr();

    _pimpl->_objectCollectorPtr.reset(new SimpleHierarchicalVisitor<PhysicalObject>
                                      ([this](PhysicalObject &object)
    {
        _pimpl->_objectVect.emplace_back(&object);
    }));
}

StrictCollisionProcessor::StrictCollisionProcessor(StrictCollisionProcessor&& /*other*/) = default;
StrictCollisionProcessor& StrictCollisionProcessor::operator=(StrictCollisionProcessor&& /*other*/) = default;
StrictCollisionProcessor::~StrictCollisionProcessor() = default;


void StrictCollisionProcessor::updateMetadata()
{
    if (getEnginePtr()->getWorldPtr() == nullptr)
        throw std::logic_error("PhysicalEngine::updateObjectCache: world is not set");

    _pimpl->_objectVect.clear();
    _pimpl->_objectCollectorPtr->visit(getEnginePtr()->getWorldPtr()->getSubObjects());
}


void StrictCollisionProcessor::processFrame(double frameTimeSec)
{
    //std::cout << "StrictCollisionProcessor::processFrame" << std::endl;

    // update state of object metadata & other service actions
    _pimpl->doPreProcess(frameTimeSec);

    // collision checking & processing
    _pimpl->processCollisions(frameTimeSec);

    // error recovery
    // _pimpl->doPostProcess(frameTimeSec);
}


void StrictCollisionProcessor::Impl::doPreProcess(double /*frameTimeSec*/)
{
    for (ObjectMetadata &metadata : _objectVect)
    {
        // reset contiguous objects
        metadata._objectPtr->resetContiguousObjects();

        // remember current position
        metadata._lastPosition = metadata._objectPtr->getPosition();

        // reset last connection number
        metadata._lastConnectionNum = 0;
    }
}


void StrictCollisionProcessor::Impl::processCollisions(double fullframeTimeSec)
{
    _totalConnectionCount = 0;
    size_t iterationCount = 0;
    double restFrameTimeSec = fullframeTimeSec;

    for (bool hasCollision = true; hasCollision; ++iterationCount)  // TODO: limit iteration count
    {
        // find the earliest collision
        CollisionInfo earliestCollision;

        for (size_t lessObjectNum = 0; lessObjectNum < _objectVect.size(); ++lessObjectNum)
            for (size_t greaterObectNum = lessObjectNum + 1; greaterObectNum < _objectVect.size(); ++greaterObectNum)
            {
                CollisionInfo possibleCollision = findCollisionBetween(lessObjectNum, greaterObectNum, restFrameTimeSec);

                if (possibleCollision._hasCollision && possibleCollision._timeRate < earliestCollision._timeRate)
                    earliestCollision = possibleCollision;
            }

        // process collision
        hasCollision = earliestCollision._hasCollision;

        if (hasCollision)
            processCollision(earliestCollision, restFrameTimeSec);

        // move objects to collision moment
        for (size_t objectNum = 0; objectNum < _objectVect.size(); ++objectNum)
            if (objectNum != earliestCollision._lessObjectNum
                    && objectNum != earliestCollision._greaterObectNum)
            {
                ObjectMetadata *metadataPtr = &_objectVect[objectNum];
                SimplePhysicalObjectPointer objPtr = metadataPtr->_objectPtr;

                //if (!metadataPtr->_isStatic)
                objPtr->setPosition(objPtr->getPosition()
                                    + objPtr->getSpeed() * restFrameTimeSec * earliestCollision._timeRate);
            }

        restFrameTimeSec *= (1 - earliestCollision._timeRate);
    }
}


void StrictCollisionProcessor::Impl::doPostProcess(double /*frameTimeSec*/)
{
    // error recovery
    // for each pair of objects
    for (size_t lessObjectNum = 0; lessObjectNum < _objectVect.size(); ++lessObjectNum)
        for (size_t greaterObjectNum = lessObjectNum + 1; greaterObjectNum < _objectVect.size(); ++greaterObjectNum)
        {
            ObjectMetadata *firstMetadataPtr  = &_objectVect[lessObjectNum];
            ObjectMetadata *secondMetadataPtr = &_objectVect[greaterObjectNum];
            SimplePhysicalObjectPointer firstObjectPtr  = firstMetadataPtr->_objectPtr;
            SimplePhysicalObjectPointer secondObjectPtr = secondMetadataPtr->_objectPtr;

            auto firstRectIt  = firstObjectPtr->getGeometry();
            auto secondRectIt = secondObjectPtr->getGeometry();
            bool isCollide = false;

            // for each pair of objects rectangles
            for (firstRectIt->first(); !firstRectIt->isDone() && !isCollide; firstRectIt->next())
                for (secondRectIt->first(); !secondRectIt->isDone() && !isCollide; secondRectIt->next())
                {
                    // calculate position of first rectangle at -ABSOLUTE_TIME_ERROR / 2 moment
                    Rectangle firstRect = firstRectIt->current();
                    firstRect.setPosition(firstRect.getPosition() + firstObjectPtr->getSpeed() * (ABSOLUTE_TIME_ERROR * -0.5));
                    firstRect = firstObjectPtr->mapToGlobal(firstRect, _worldPtr.get());

                    // calculate position of second rectangle at -ABSOLUTE_TIME_ERROR / 2 moment
                    Rectangle secondRect = secondRectIt->current();
                    secondRect.setPosition(secondRect.getPosition() + secondObjectPtr->getSpeed() * (ABSOLUTE_TIME_ERROR * -0.5));
                    secondRect = secondObjectPtr->mapToGlobal(secondRect, _worldPtr.get());

                    // if rectangles collide then there is an error here
                    isCollide = firstRect.isStrongCollided(secondRect);
                }

            // in case of error return objects to the last position
            if (isCollide)
            {
                firstObjectPtr->setPosition(firstMetadataPtr->_lastPosition);
                secondObjectPtr->setPosition(secondMetadataPtr->_lastPosition);
            }
        }
}


void StrictCollisionProcessor::Impl::processCollision(const CollisionInfo &collision, double frameTimeSec)
{
    ObjectMetadata *firstMetadataPtr  = &_objectVect[collision._lessObjectNum];
    ObjectMetadata *secondMetadataPtr = &_objectVect[collision._greaterObectNum];
    SimplePhysicalObjectPointer firstObjPtr  = firstMetadataPtr->_objectPtr;
    SimplePhysicalObjectPointer secondObjPtr = secondMetadataPtr->_objectPtr;
    const bool isHorizontalCollision = collision._direction == Right || collision._direction == Left;

    // move objects to collision point
     firstObjPtr->setPosition(firstObjPtr->getPosition()   + firstObjPtr->getSpeed()  * frameTimeSec * collision._timeRate);
     secondObjPtr->setPosition(secondObjPtr->getPosition() + secondObjPtr->getSpeed() * frameTimeSec * collision._timeRate);

    // set contiguous objects
    SimplePhysicalObjectPointer lastNeighborPtr = firstObjPtr->getContiguousObject(collision._direction);
    firstObjPtr->setContiguousObject(collision._direction, secondObjPtr);
    secondObjPtr->setContiguousObject(getOppositeDirrection(collision._direction), firstObjPtr);

    // set connection number
    if (lastNeighborPtr != secondObjPtr)
    {
        _totalConnectionCount++;
        firstMetadataPtr->_lastConnectionNum = _totalConnectionCount;
    }

    bool hasSameConnectionNum = firstMetadataPtr->_lastConnectionNum
                            == secondMetadataPtr->_lastConnectionNum;
    size_t maxConnectionNum = std::max(firstMetadataPtr->_lastConnectionNum,
                                       secondMetadataPtr->_lastConnectionNum);
    firstMetadataPtr->_lastConnectionNum  = maxConnectionNum;
    secondMetadataPtr->_lastConnectionNum = maxConnectionNum;

    if (hasSameConnectionNum)
    {
        // simplified collision processing
        double firstObjectSpeed  = firstObjPtr->getSpeed().getProjection(isHorizontalCollision);
        double secondObjectSpeed = secondObjPtr->getSpeed().getProjection(isHorizontalCollision);
        double newSpeed = 0;

        if (!firstObjPtr->isMovable())
            newSpeed = firstObjectSpeed;
        else if (!secondObjPtr->isMovable())
            newSpeed = secondObjectSpeed;
        else
            newSpeed = 0;

        Point firstObjectSpeedVect = firstObjPtr->getSpeed();
        firstObjectSpeedVect.setProjection(newSpeed, isHorizontalCollision);
        firstObjPtr->setSpeed(firstObjectSpeedVect);

        Point secondObjectSpeedVect = secondObjPtr->getSpeed();
        secondObjectSpeedVect.setProjection(newSpeed, isHorizontalCollision);
        secondObjPtr->setSpeed(secondObjectSpeedVect);
        return;
    }

    // calculate new speeds
    const double speedRecoveryFactor = (   firstObjPtr->getHitRecoveryFactor()
                                        + secondObjPtr->getHitRecoveryFactor()) / 2;

    if (firstObjPtr->isMovable() && secondObjPtr->isMovable())
    {
        // hit between two objects
        double firstObjectSpeed  = firstObjPtr->getSpeed().getProjection(isHorizontalCollision);
        double secondObjectSpeed = secondObjPtr->getSpeed().getProjection(isHorizontalCollision);
        double newFirstObjectSpeed  = 0;
        double newSecondObjectSpeed = 0;

        solveCentralHit(firstObjectSpeed, firstObjPtr->getMass(),
                        secondObjectSpeed, secondObjPtr->getMass(),
                        speedRecoveryFactor,
                        newFirstObjectSpeed, newSecondObjectSpeed);

        Point firstObjectSpeedVect = firstObjPtr->getSpeed();
        firstObjectSpeedVect.setProjection(newFirstObjectSpeed, isHorizontalCollision);
        firstObjPtr->setSpeed(firstObjectSpeedVect);

        Point secondObjectSpeedVect = secondObjPtr->getSpeed();
        secondObjectSpeedVect.setProjection(newSecondObjectSpeed, isHorizontalCollision);
        secondObjPtr->setSpeed(secondObjectSpeedVect);
    }
    else if (firstObjPtr->isMovable() || secondObjPtr->isMovable())
    {
        // hit between object and platform
        SimplePhysicalObjectPointer objectPtr   = ( firstObjPtr->isMovable() ? firstObjPtr : secondObjPtr);
        SimplePhysicalObjectPointer platformPtr = (!firstObjPtr->isMovable() ? firstObjPtr : secondObjPtr);
        double objectSpeed   = objectPtr->getSpeed().getProjection(isHorizontalCollision);
        double platformSpeed = platformPtr->getSpeed().getProjection(isHorizontalCollision);
        double newObjectSpeed   = 0;

        solvePlatformHit(objectSpeed, platformSpeed, speedRecoveryFactor, newObjectSpeed);

        Point objectSpeedVect = objectPtr->getSpeed();
        objectSpeedVect.setProjection(newObjectSpeed, isHorizontalCollision);
        objectPtr->setSpeed(objectSpeedVect);
    }
    else
    {
        // do nothing
    }
}


CollisionInfo StrictCollisionProcessor::Impl::findCollisionBetween(size_t lessObjectNum,
                                                                   size_t greaterObjectNum,
                                                                   double frameTimeSec)
{
    ObjectMetadata *firstMetadataPtr  = &_objectVect[lessObjectNum];
    ObjectMetadata *secondMetadataPtr = &_objectVect[greaterObjectNum];
    SimplePhysicalObjectPointer firstObjPtr  = firstMetadataPtr->_objectPtr;
    SimplePhysicalObjectPointer secondObjPtr = secondMetadataPtr->_objectPtr;

    if (!firstObjPtr->isMovable() && !secondObjPtr->isMovable())
        return CollisionInfo();

    RectangleIteratorPtr firstRectIterator  = firstObjPtr->getGeometry();
    RectangleIteratorPtr secondRectIterator = secondObjPtr->getGeometry();

    // ===== not refactored ==============
    double minCollisionTime = 1;
    bool isHorizontalCollision = false;

    static double timeVect[4];
    static const bool TYPE_FLAGS[4] = {false, false, true, true};

    for (firstRectIterator->first(); !firstRectIterator->isDone(); firstRectIterator->next() /*, ++rectNum*/)
    {
        Rectangle rect1 = firstObjPtr->mapToGlobal(firstRectIterator->current(), _worldPtr.get());
        Rectangle rect1M(rect1.getPosition() + firstObjPtr->getSpeed() * frameTimeSec, rect1.getSize());

        for (secondRectIterator->first(); !secondRectIterator->isDone(); secondRectIterator->next())
        {
            Rectangle rect2 = secondObjPtr->mapToGlobal(secondRectIterator->current(), _worldPtr.get());
            Rectangle rect2M(rect2.getPosition() + secondObjPtr->getSpeed() * frameTimeSec, rect2.getSize());
            double tbt = (rect2.getTop() - rect1.getBottom())
                    / (rect1M.getBottom() - rect1.getBottom() - rect2M.getTop() + rect2.getTop());
            double ttb = (rect1.getTop() - rect2.getBottom())
                    / (rect2M.getBottom() - rect2.getBottom() - rect1M.getTop() + rect1.getTop());
            double trl = (rect2.getLeft() - rect1.getRight())
                    / (rect1M.getRight() - rect1.getRight() - rect2M.getLeft() + rect2.getLeft());
            double tlr = (rect1.getLeft() - rect2.getRight())
                    / (rect2M.getRight() - rect2.getRight() - rect1M.getLeft() + rect1.getLeft());

            timeVect[0] = tbt;
            timeVect[1] = ttb;
            timeVect[2] = trl;
            timeVect[3] = tlr;

            double minHTime = 1;
            double minVTime = 1;

            for (int tnum = 0; tnum < 4; ++tnum)
                if (timeVect[tnum] >= -ABSOLUTE_TIME_ERROR && timeVect[tnum] < minCollisionTime)
                {
                    if (TYPE_FLAGS[tnum])
                         minHTime = timeVect[tnum];
                    else minVTime = timeVect[tnum];
                }

            bool hasHCollision = minHTime < 1;
            bool hasVCollision = minVTime < 1;

            hasHCollision &= (firstObjPtr->getSpeed().getX() != secondObjPtr->getSpeed().getX());
            hasVCollision &= (firstObjPtr->getSpeed().getY() != secondObjPtr->getSpeed().getY());

            // check that collision has really happened
            double x1 = rect1.getLeft() + firstObjPtr->getSpeed().getX() * frameTimeSec * minVTime
                    + rect1.getWidth() / 2;
            double x2 = rect2.getLeft() + secondObjPtr->getSpeed().getX() * frameTimeSec * minVTime
                    + rect2.getWidth() / 2;
            double y1 = rect1.getTop() + firstObjPtr->getSpeed().getY() * frameTimeSec * minHTime
                    + rect1.getHeight() / 2;
            double y2 = rect2.getTop() + secondObjPtr->getSpeed().getY() * frameTimeSec * minHTime
                    + rect2.getHeight() / 2;

            hasVCollision &= abs(x1 - x2) <= (rect1.getWidth()  + rect2.getWidth())  / 2;
            hasHCollision &= abs(y1 - y2) <= (rect1.getHeight() + rect2.getHeight()) / 2;

            hasHCollision &= timeVect[2] >= -ABSOLUTE_TIME_ERROR && timeVect[3] >= -ABSOLUTE_TIME_ERROR;
            hasVCollision &= timeVect[0] >= -ABSOLUTE_TIME_ERROR && timeVect[1] >= -ABSOLUTE_TIME_ERROR;

            // Debug output
#ifdef DEBUG_OUTPUT
            std::cout << objNum1 << "-" << objNum2 << " " << x1 << " " << x2 << " {";
            for (int tnum = 0; tnum < 4; ++tnum)
                std::cout << timeVect[tnum] << ", ";
            std::cout << "} " << hasHCollision << " " << hasVCollision << " " << isHorizontalCollision << std::endl;
#endif

            if (hasHCollision || hasVCollision)
            {
                minCollisionTime = std::min(minHTime, minVTime);
                isHorizontalCollision = minHTime < minVTime;
            }
        }
    }

    Direction direction;

    if (isHorizontalCollision)
         direction = (firstObjPtr->getPosition().getX() < secondObjPtr->getPosition().getX()) ? Right : Left;
    else direction = (firstObjPtr->getPosition().getY() < secondObjPtr->getPosition().getY()) ? Down : Up;
    // ===================

    CollisionInfo collision;
    collision._direction = direction;
    collision._lessObjectNum = lessObjectNum;
    collision._greaterObectNum = greaterObjectNum;
    collision._hasCollision = minCollisionTime < 1;
    collision._timeRate = minCollisionTime;
    return collision;
}


void StrictCollisionProcessor::Impl::solvePlatformHit(double objectSpeed, double platformSpeed,
                                                      double speedRecoveryFactor,
                                                      double &newObjectSpeed) const
{
    newObjectSpeed = platformSpeed - speedRecoveryFactor * (objectSpeed - platformSpeed);
}


void StrictCollisionProcessor::Impl::solveCentralHit(double firstObjectSpeed,     double firstObjectMass,
                                                     double secondObjectSpeed,    double secondObjectMass,
                                                     double speedRecoveryFactor,
                                                     double &newFirstObjectSpeed, double &newSecondObjectSpeed) const
{
    double speedDifference = firstObjectSpeed - secondObjectSpeed;
    newFirstObjectSpeed = (firstObjectMass  * firstObjectSpeed
                           + secondObjectMass * secondObjectSpeed
                           - secondObjectMass * speedRecoveryFactor * speedDifference)
            / (firstObjectMass + secondObjectMass);

    newSecondObjectSpeed = newFirstObjectSpeed + speedRecoveryFactor * speedDifference;
}




}  // namespace Platformer
