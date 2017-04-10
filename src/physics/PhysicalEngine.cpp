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


namespace Platformer
{


struct ObjectMetadata
{
    ObjectMetadata(SimplePhysicalObjectPointer objectPtr)
        : _objectPtr(objectPtr)
    {
    }

    SimplePhysicalObjectPointer _objectPtr = nullptr;
    Point _lastPosition;
    size_t _staticFrameCount = 0;
    ObjectMetadata *_contiguousObjects[4] = {nullptr, nullptr, nullptr, nullptr};
    bool _isStatic = false;
    bool _isStand = false;
};


//struct CollisionInfo
//{
//    size_t _lessObjectNum, _largerObectNum;
//    double _time;
//    bool _hasCollision;
//    Direction _direction;
//};



struct PhysicalEngine::Impl
{
    using ObjectIteratorPtr = Pointer<Iterator<SimplePhysicalObjectPointer>>;

    // processing steps
    void updateObjectMetadata(ObjectMetadata *metadataPtr);
    void applyPhisicalRules(SimplePhysicalObjectPointer objPtr);
    void performPostProcess();

    void processCollision(size_t objNum1, size_t objNum2,
                          double frameTime, double collisionTimeFactor, Direction direction);

    // important functions
    std::pair<double, Direction> getCollisionTime(size_t objNum1, size_t objNum2, double frameTime);

    std::pair<double, double> solveCentralHit(double speed1, double mass1, double speed2, double mass2);

    void applyFrictionBetween(SimplePhysicalObjectPointer obj1Ptr,
                              SimplePhysicalObjectPointer obj2Ptr,
                              bool isHorizontal, double frameTime);


    void activate(ObjectMetadata *metadataPtr,
                  ObjectMetadata *parentMetadataPtr);
    void processStand(ObjectMetadata *metadataPtr);


    PhysicalWorldPointer _worldPtr;
    std::vector<ObjectMetadata> _objectVect;
    SimpleHierarchicalVisitorPointer<PhysicalObject> _objectCollectorPtr;

    double _g = 2000.0;
    double _airFrictionFactor = 50;
    double _minSpeed = 0.5;
    double _maxSpeed = 5000;
    double _minDistance = 0.1;
    size_t _staticFrameCount = 50;
};


//#define DEBUG_OUTPUT

// constructors, destructor and so on

PhysicalEngine::PhysicalEngine()
    : _pimpl(new Impl())
{
    _pimpl->_objectCollectorPtr.reset(new SimpleHierarchicalVisitor<PhysicalObject>
                                      ([this](PhysicalObject &object)
    {
        _pimpl->_objectVect.emplace_back(&object);
    }));
}


PhysicalEngine::PhysicalEngine(PhysicalEngine&& /*other*/) = default;
PhysicalEngine& PhysicalEngine::operator=(PhysicalEngine&& /*other*/) = default;
PhysicalEngine::~PhysicalEngine() = default;


// main method

void PhysicalEngine::processWorld()
{
    double frameTime = Platform::instance()->getActualFrameTime();

    // update static states
    for (size_t objectNum = 0; objectNum < _pimpl->_objectVect.size(); ++objectNum)
    {
        ObjectMetadata *metadataPtr = &_pimpl->_objectVect[objectNum];
        SimplePhysicalObjectPointer objPtr = metadataPtr->_objectPtr;
        Point d = objPtr->getPosition() + metadataPtr->_lastPosition * (-1);
        metadataPtr->_lastPosition = objPtr->getPosition();

        if (d.getLength() < getMinDistance() && objPtr->getSpeed().getLength() * frameTime < getMinDistance())
            metadataPtr->_staticFrameCount++;
        else
            metadataPtr->_staticFrameCount = 0;

        metadataPtr->_isStatic = metadataPtr->_staticFrameCount >= getStaticFrameCount();
    }

    // reset service metadata
    for (ObjectMetadata &metadata : _pimpl->_objectVect)
    {
        metadata._isStand = false;

        // reset contiguous objects
        metadata._objectPtr->resetContiguousObjects();

        for (long num : Range(4))
            metadata._contiguousObjects[num] = nullptr;
    }

    // apply phisical rules
    for (ObjectMetadata &metadata : _pimpl->_objectVect)
    {
        SimplePhysicalObjectPointer objPtr = metadata._objectPtr;

        if (!metadata._isStatic)
        {
            // apply gravity
            if (objPtr->isMovable())
                objPtr->setSpeed(objPtr->getSpeed() + Point(0, getG() * frameTime));

            // apply friction
            SimplePhysicalObjectPointer downObjPtr = objPtr->getContiguousObject(Down);
            SimplePhysicalObjectPointer rightObjPtr = objPtr->getContiguousObject(Right);

            _pimpl->applyFrictionBetween(objPtr, downObjPtr, false, frameTime);
            _pimpl->applyFrictionBetween(objPtr, rightObjPtr, true, frameTime);

            // limit speeds & apply air friction
            Point speedV = objPtr->getSpeed();
            double speed = std::sqrt(speedV.getX() * speedV.getX() + speedV.getY() * speedV.getY());

            int signX = (speedV.getX() > 0) ? 1 : ((speedV.getX() < 0) ? -1 : 0);
            int signY = (speedV.getY() > 0) ? 1 : ((speedV.getY() < 0) ? -1 : 0);
            objPtr->setSpeed(speedV + Point(-signX, -signY) * getAirFrictionFactor() * frameTime);

            if (speed < getMinSpeed())
                objPtr->setSpeed(Point(0, 0));

            if (speed > getMaxSpeed())
                objPtr->setSpeed(speedV * (getMaxSpeed() / speed));
        }
    }

    // collision check
    size_t count = 0;

    for (bool hasCollision = true; hasCollision /*&& count < 2*/; ++count)
    {
        // find the earliest collision
        double minCollisionTime = 1;
        Direction direction = Right;
        size_t collObjectNum1 = _pimpl->_objectVect.size();
        size_t collObjectNum2 = _pimpl->_objectVect.size();

        for (size_t objectNum = 0; objectNum < _pimpl->_objectVect.size(); ++objectNum)
        {
            for (size_t objectNum2 = objectNum + 1; objectNum2 < _pimpl->_objectVect.size(); ++objectNum2)
            {
                auto pair = _pimpl->getCollisionTime(objectNum, objectNum2, frameTime);

                if (pair.first < minCollisionTime)
                {
                    minCollisionTime = pair.first;
                    direction = pair.second;
                    collObjectNum1 = objectNum;
                    collObjectNum2 = objectNum2;
                }
            }
        }

        // process collision
        hasCollision = (minCollisionTime < 1);

        if (hasCollision)
        {
            // Debug output
#ifdef DEBUG_OUTPUT
            SimplePhysicalObjectPointer objPtr1 = _pimpl->_objectVect[collObjectNum1];
            SimplePhysicalObjectPointer objPtr2 = _pimpl->_objectVect[collObjectNum2];
            auto speed1 = objPtr1->getSpeed().getY();
            auto speed2 = objPtr2->getSpeed().getY();
            auto pos1 = objPtr1->getPosition().getY();
            auto pos2 = objPtr2->getPosition().getY();
#endif
            _pimpl->processCollision(collObjectNum1, collObjectNum2,
                                     frameTime, minCollisionTime, direction);

            // Debug output
#ifdef DEBUG_OUTPUT
            std::cout << minCollisionTime << " " << direction << " " << objPtr1->isStand() << " " << objPtr2->isStand() << std::endl;
            std::cout << "    " << collObjectNum1 << ": sy " << speed1 << " -> " << objPtr1->getSpeed().getY()
                      << ", py " << pos1 << " -> " << objPtr1->getPosition().getY() << std::endl;
            std::cout << "    " << collObjectNum2 << ": sy " << speed2 << " -> " << objPtr2->getSpeed().getY()
                      << ", py " << pos2 << " -> " << objPtr2->getPosition().getY()<< std::endl;
            //std::cout << std::endl;
#endif
        }

        // move objects
        for (size_t objectNum = 0; objectNum < _pimpl->_objectVect.size(); ++objectNum)
            if (objectNum != collObjectNum1 && objectNum != collObjectNum2)
            {
                ObjectMetadata *metadataPtr = &_pimpl->_objectVect[objectNum];
                SimplePhysicalObjectPointer objPtr = metadataPtr->_objectPtr;

                if (!metadataPtr->_isStatic)
                    objPtr->setPosition(objPtr->getPosition()
                                        + objPtr->getSpeed()
                                        * frameTime * minCollisionTime);
            }

            frameTime *= (1 - minCollisionTime);
        }

    // Debug output
#ifdef DEBUG_OUTPUT
    //if (count > 1)
        std::cout << std::endl;
#endif

    // additional collision check for debug
    for (size_t objectNum1 = 0; objectNum1 < _pimpl->_objectVect.size(); ++objectNum1)
        for (size_t objectNum2 = objectNum1 + 1; objectNum2 < _pimpl->_objectVect.size(); ++objectNum2)
        {
            ObjectMetadata *metadataPtr1 = &_pimpl->_objectVect[objectNum1];
            ObjectMetadata *metadataPtr2 = &_pimpl->_objectVect[objectNum2];
            SimplePhysicalObjectPointer objPtr1 = metadataPtr1->_objectPtr;
            SimplePhysicalObjectPointer objPtr2 = metadataPtr2->_objectPtr;

            auto it1 = objPtr1->getGeometry();
            auto it2 = objPtr2->getGeometry();
            bool isCollide = false;

            for (it1->first(); !it1->isDone() && !isCollide; it1->next())
                for (it2->first(); !it2->isDone() && !isCollide; it2->next())
                {
                    Rectangle rect1 = objPtr1->mapToGlobal(it1->current(), _pimpl->_worldPtr.get());
                    Rectangle rect2 = objPtr2->mapToGlobal(it2->current(), _pimpl->_worldPtr.get());

                    isCollide = rect1.isStrongCollided(rect2);
                }

            if (isCollide)
            {
                objPtr1->setPosition(metadataPtr1->_lastPosition);
                objPtr2->setPosition(metadataPtr2->_lastPosition);
                //std::cout << "Here" << std::endl;
            }
        }
}


std::pair<double, Direction> PhysicalEngine::Impl::getCollisionTime(size_t objNum1, size_t objNum2, double frameTime)
{
//    SimplePhysicalObjectPointer objPtr1 = _objectVect[objNum1];
//    SimplePhysicalObjectPointer objPtr2 = _objectVect[objNum2];

    ObjectMetadata *metadataPtr1 = &_objectVect[objNum1];
    ObjectMetadata *metadataPtr2 = &_objectVect[objNum2];
    SimplePhysicalObjectPointer objPtr1 = metadataPtr1->_objectPtr;
    SimplePhysicalObjectPointer objPtr2 = metadataPtr2->_objectPtr;

    if (metadataPtr1->_isStatic && metadataPtr2->_isStatic)
        return std::make_pair(1, Right);

    if (!objPtr1->isMovable() && !objPtr2->isMovable())
        return std::make_pair(1, Right);

    RectangleIteratorPtr rectIt1 = objPtr1->getGeometry();
    RectangleIteratorPtr rectIt2 = objPtr2->getGeometry();
    size_t rectNum = 0;
    double minCollisionTime = 1;
    bool isHorizontalCollision = false;

    static double timeVect[4];
    static const bool TYPE_FLAGS[4] = {false, false, true, true};

    for (rectIt1->first(); !rectIt1->isDone(); rectIt1->next(), ++rectNum)
    {
        Rectangle rect1 = objPtr1->mapToGlobal(rectIt1->current(), _worldPtr.get());
        Rectangle rect1M(rect1.getPosition() + objPtr1->getSpeed() * frameTime, rect1.getSize());

        for (rectIt2->first(); !rectIt2->isDone(); rectIt2->next())
        {
            Rectangle rect2 = objPtr2->mapToGlobal(rectIt2->current(), _worldPtr.get());
            Rectangle rect2M(rect2.getPosition() + objPtr2->getSpeed() * frameTime, rect2.getSize());
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

            static const double EPS = 0.0001;

            double minHTime = 1; //std::min(timeVect[2], timeVect[3]);
            double minVTime = 1; //std::min(timeVect[0], timeVect[1]);

            for (int tnum = 0; tnum < 4; ++tnum)
                if (timeVect[tnum] >= -EPS && timeVect[tnum] < minCollisionTime)
                {
                    if (TYPE_FLAGS[tnum])
                         minHTime = timeVect[tnum];
                    else minVTime = timeVect[tnum];
                }

            bool hasHCollision = minHTime < 1;
            bool hasVCollision = minVTime < 1;

//            if (objPtr1->isStand() && objPtr2->isStand())

            if (metadataPtr1->_isStand && metadataPtr2->_isStand)
            {
                hasVCollision = false;
                minVTime = 1;
            }

            hasHCollision &= (objPtr1->getSpeed().getX() != objPtr2->getSpeed().getX());
            hasVCollision &= (objPtr1->getSpeed().getY() != objPtr2->getSpeed().getY());

            // check that collision is really happened
            double x1 = rect1.getLeft() + objPtr1->getSpeed().getX() * frameTime * minVTime
                    + rect1.getWidth() / 2;
            double x2 = rect2.getLeft() + objPtr2->getSpeed().getX() * frameTime * minVTime
                    + rect2.getWidth() / 2;
            double y1 = rect1.getTop() + objPtr1->getSpeed().getY() * frameTime * minHTime
                    + rect1.getHeight() / 2;
            double y2 = rect2.getTop() + objPtr2->getSpeed().getY() * frameTime * minHTime
                    + rect2.getHeight() / 2;

            hasVCollision &= std::abs(x1 - x2) <= (rect1.getWidth()  + rect2.getWidth())  / 2;
            hasHCollision &= std::abs(y1 - y2) <= (rect1.getHeight() + rect2.getHeight()) / 2;

            hasHCollision &= timeVect[2] >= -EPS && timeVect[3] >= -EPS;
            hasVCollision &= timeVect[0] >= -EPS && timeVect[1] >= -EPS;

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
         direction = (objPtr1->getPosition().getX() < objPtr2->getPosition().getX()) ? Right : Left;
    else direction = (objPtr1->getPosition().getY() < objPtr2->getPosition().getY()) ? Down : Up;

    return std::make_pair(minCollisionTime, direction);
}


void PhysicalEngine::Impl::processCollision(size_t objNum1, size_t objNum2,
                                            double frameTime,
                                            double collisionTimeFactor,
                                            Direction direction)
{

//    SimplePhysicalObjectPointer objPtr1 = _objectVect[objNum1];
//    SimplePhysicalObjectPointer objPtr2 = _objectVect[objNum2];
    ObjectMetadata *metadataPtr1 = &_objectVect[objNum1];
    ObjectMetadata *metadataPtr2 = &_objectVect[objNum2];
    SimplePhysicalObjectPointer objPtr1 = metadataPtr1->_objectPtr;
    SimplePhysicalObjectPointer objPtr2 = metadataPtr2->_objectPtr;

    const bool isHorizontalCollision = direction == Right || direction == Left;
    Point speed1V = objPtr1->getSpeed();
    Point speed2V = objPtr2->getSpeed();

//    activate(objPtr1, objPtr2);
//    activate(objPtr2, objPtr1);
    activate(metadataPtr1, metadataPtr2);
    activate(metadataPtr2, metadataPtr1);

    // move objects to collision point
    objPtr1->setPosition(objPtr1->getPosition() + speed1V * frameTime * collisionTimeFactor);
    objPtr2->setPosition(objPtr2->getPosition() + speed2V * frameTime * collisionTimeFactor);

    // calculate new speeds
    const double ENERGY_LOST_FACTOR
            = (objPtr1->getHitRecoveryFactor() + objPtr2->getHitRecoveryFactor()) / 2;

    if (!objPtr1->isMovable() || !objPtr2->isMovable())
    {
        SimplePhysicalObjectPointer objPtr = (objPtr1->isMovable() ? objPtr1 : objPtr2);
        Point speedV = objPtr->getSpeed();

        double speed = isHorizontalCollision ? speedV.getX() : speedV.getY();
        int sign = speed > 0 ? 1 : -1;
        double v = std::abs(speed);
        double e = objPtr->getMass() * v * v / 2;
        double de = e * (1 - ENERGY_LOST_FACTOR);
        double dv = std::sqrt(2 * de / objPtr->getMass());

        if (isHorizontalCollision)
             objPtr->setSpeed(Point(-speedV.getX() + dv * sign, speedV.getY()));
        else objPtr->setSpeed(Point(speedV.getX(), -speedV.getY() + dv * sign));
    }
    else
    {
        std::pair<double, double> speedPair;

        if (isHorizontalCollision)
        {
            speedPair = solveCentralHit(speed1V.getX(), objPtr1->getMass(),
                                        speed2V.getX(), objPtr2->getMass());

            int sign1 = speedPair.first  > 0 ? 1 : -1;
            int sign2 = speedPair.second > 0 ? 1 : -1;
            double v = std::abs(speedPair.first - speedPair.second);
            double e = (objPtr1->getMass() + objPtr2->getMass()) * v * v / 4;
            double de = e * (1 - ENERGY_LOST_FACTOR);
            double de1 = de * std::abs(speedPair.first)  / (std::abs(speedPair.first) + std::abs(speedPair.second));
            double de2 = de * std::abs(speedPair.second) / (std::abs(speedPair.first) + std::abs(speedPair.second));
            double dv1 = std::sqrt(2 * de1 / objPtr1->getMass());
            double dv2 = std::sqrt(2 * de2 / objPtr1->getMass());

            speed1V.setX(speedPair.first  - dv1 * sign1);
            speed2V.setX(speedPair.second - dv2 * sign2);
        }
        else
        {
            speedPair = solveCentralHit(speed1V.getY(), objPtr1->getMass(),
                                        speed2V.getY(), objPtr2->getMass());

            int sign1 = speedPair.first  > 0 ? 1 : -1;
            int sign2 = speedPair.second > 0 ? 1 : -1;
            double v = std::abs(speedPair.first - speedPair.second);
            double e = (objPtr1->getMass() + objPtr2->getMass()) * v * v / 4;
            double de = e * (1 - ENERGY_LOST_FACTOR);
            double de1 = de * std::abs(speedPair.first)  / (std::abs(speedPair.first) + std::abs(speedPair.second));
            double de2 = de * std::abs(speedPair.second) / (std::abs(speedPair.first) + std::abs(speedPair.second));
            double dv1 = std::sqrt(2 * de1 / objPtr1->getMass());
            double dv2 = std::sqrt(2 * de2 / objPtr1->getMass());

            speed1V.setY(speedPair.first  - dv1 * sign1);
            speed2V.setY(speedPair.second - dv2 * sign2);
        }

        objPtr1->setSpeed(speed1V);
        objPtr2->setSpeed(speed2V);
    }

    objPtr1->setContiguousObject(direction, objPtr2);
    objPtr2->setContiguousObject(getOppositeDirrection(direction), objPtr1);
    metadataPtr1->_contiguousObjects[direction] = metadataPtr2;
    metadataPtr2->_contiguousObjects[getOppositeDirrection(direction)] = metadataPtr1;

    processStand(metadataPtr1);
    processStand(metadataPtr2);
}


void PhysicalEngine::Impl::processStand(/*SimplePhysicalObjectPointer objPtr*/
                                        ObjectMetadata *metadataPtr)
{
    SimplePhysicalObjectPointer objPtr = metadataPtr->_objectPtr;
    ObjectMetadata *downMetadataPtr = metadataPtr->_contiguousObjects[Down];

    if (!objPtr->isMovable() || downMetadataPtr == nullptr)
        return;

    SimplePhysicalObjectPointer downObjPtr = downMetadataPtr->_objectPtr; //objPtr->getContiguousObject(Down);

    if (!downObjPtr->isMovable() || downMetadataPtr->_isStand)
    {
//        objPtr->setIsStand(true);
        metadataPtr->_isStand = true;
        objPtr->setSpeed(Point(objPtr->getSpeed().getX(), 0));
    }
}


void PhysicalEngine::Impl::activate(ObjectMetadata *metadataPtr,
                                    ObjectMetadata *parentMetadataPtr)
{
//    SimplePhysicalObjectPointer objPtr = metadataPtr->_objectPtr;
//    SimplePhysicalObjectPointer parentPtr = parentMetadataPtr->_objectPtr;

//    if (objPtr == nullptr || objPtr->getStaticFrameCount() <= parentPtr->getStaticFrameCount()
    if (metadataPtr == nullptr || metadataPtr->_staticFrameCount <= parentMetadataPtr->_staticFrameCount //parentPtr->getStaticFrameCount()
            || !metadataPtr->_objectPtr->isMovable())
        return;

    metadataPtr->_isStatic = false;
    metadataPtr->_staticFrameCount = parentMetadataPtr->_staticFrameCount;
    activate(metadataPtr->_contiguousObjects[Down],     metadataPtr);
    activate(metadataPtr->_contiguousObjects[Up],       metadataPtr);
    activate(metadataPtr->_contiguousObjects[Right],    metadataPtr);
    activate(metadataPtr->_contiguousObjects[Left],     metadataPtr);

    /*
    objPtr->setIsStatic(false);
    objPtr->setStaticFrameCount(//std::min(objPtr->getStaticFrameCount(),
                                         parentPtr->getStaticFrameCount());

    activate(objPtr->getContiguousObject(Down), objPtr);
    activate(objPtr->getContiguousObject(Up), objPtr);
    activate(objPtr->getContiguousObject(Right), objPtr);
    activate(objPtr->getContiguousObject(Left), objPtr);
    */
}


// phisical calculations

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


std::pair<double, double> PhysicalEngine::Impl::solveCentralHit(double speed1, double mass1,
                                                                double speed2, double mass2)
{
    double v1 = ((mass1 - mass2) * speed1 + 2 * mass2 * speed2) / (mass1 + mass2);
    double v2 = ((mass2 - mass1) * speed2 + 2 * mass1 * speed1) / (mass1 + mass2);
    return std::make_pair(v1, v2);
}


// public procedures

void PhysicalEngine::updateMetadata()
{
    if (getWorldPtr() == nullptr)
        throw std::logic_error("PhysicalEngine::updateObjectCache: world is not set");

    _pimpl->_objectVect.clear();
    _pimpl->_objectCollectorPtr->visit(getWorldPtr()->getSubObjects());
}


// getters

PhysicalWorldPointer PhysicalEngine::getWorldPtr() const
{
    return _pimpl->_worldPtr;
}

double PhysicalEngine::getG() const
{
    return _pimpl->_g;
}

double PhysicalEngine::getAirFrictionFactor() const
{
    return _pimpl->_airFrictionFactor;
}

double PhysicalEngine::getMinSpeed() const
{
    return _pimpl->_minSpeed;
}

double PhysicalEngine::getMaxSpeed() const
{
    return _pimpl->_maxSpeed;
}

double PhysicalEngine::getMinDistance() const
{
    return _pimpl->_minDistance;
}

size_t PhysicalEngine::getStaticFrameCount() const
{
    return _pimpl->_staticFrameCount;
}

double PhysicalEngine::getDefaultFirictionFactor()
{
    return 50; //200;
}

double PhysicalEngine::getDefaultHitRecoveryFactor()
{
    return 0.5;
}


// setters

void PhysicalEngine::setWorldPtr(PhysicalWorldPointer worldPtr)
{
    _pimpl->_worldPtr = worldPtr;
    updateMetadata();
}

void PhysicalEngine::setG(double g)
{
    _pimpl->_g = g;
}

void PhysicalEngine::setAirFrictionFactor(double factor)
{
    _pimpl->_airFrictionFactor = factor;
}

void PhysicalEngine::setMinSpeed(double speed)
{
    _pimpl->_minSpeed = speed;
}

void PhysicalEngine::setMaxSpeed(double speed)
{
    _pimpl->_maxSpeed = speed;
}

void PhysicalEngine::setMinDistance(double distance)
{
    _pimpl->_minDistance = distance;
}

void PhysicalEngine::setStaticFrameCount(size_t count)
{
    _pimpl->_staticFrameCount = count;
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
