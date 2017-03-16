// TestObject.h

#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include "PhysicalObject.h"


namespace Platformer
{


class TestObject : public PhysicalObject
{
public:
    TestObject(double mass = 10, const std::string &name = "[TestObject]");
    TestObject(const std::string &name);
    TestObject(TestObject&& other);
    virtual TestObject& operator=(TestObject&& other);
    virtual ~TestObject();

    virtual double getMass() const override;
    virtual RectangleIteratorPtr getGeometry() const override;

    void setMass(double mass);
    void setSize(const Point &size);
    virtual void accept(GameObjectVisitor& visitor) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // TESTOBJECT_H
