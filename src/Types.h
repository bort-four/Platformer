// Types.h

#ifndef TYPES_H
#define TYPES_H

#include <memory>

#include "Iterator.h"


namespace Platformer
{

class Point;
class Rectangle;
class GameObject;
class GameObjectContainer;
class PhysicalWorld;
class PhysicalObject;
class TestObject;
class GameObjectVisitor;
class Rectangle;
class Game;
class PlatformManager;
class Visualizer;
class GamePainter;
class Key;
class PhysicalEngine;
class HierarchicalVisitor;
class MapPlatform;
class CollisionProcessor;
class StrictCollisionProcessor;

template <class ValueType> using Pointer = std::shared_ptr<ValueType>;
template <class BaseNodeType> class VisitorBase;
template <class PrecessedNodeType> class SimpleVisitor;
template <class PrecessedNodeType> class SimpleHierarchicalVisitor;

using PointPointer = Pointer<Point>;
using RectanglePointer = Pointer<Rectangle>;
using GameObjectPointer = Pointer<GameObject>;
using ConstSimpleGameObjectPointer = GameObject const *;
using GameObjectContainerPointer = Pointer<GameObjectContainer>;
using PhysicalWorldPointer = Pointer<PhysicalWorld>;
using PhysicalObjectPointer = Pointer<PhysicalObject>;
using TestObjectPointer = Pointer<TestObject>;
using GameObjectVisitorPointer = Pointer<GameObjectVisitor>;
using GamePointer = Pointer<Game>;
using PlatformManagerPointer = Pointer<PlatformManager>;
using VisualizerPointer = Pointer<Visualizer>;
using GamePainterPointer = Pointer<GamePainter>;
using KeyPointer = Pointer<Key>;
using PhysicalEnginePointer = Pointer<PhysicalEngine>;
using SimplePhysicalEnginePointer = PhysicalEngine*;
using HierarchicalVisitorPointer = Pointer<HierarchicalVisitor>;
using MapPlatformPointer = Pointer<MapPlatform>;
using CollisionProcessorPointer = Pointer<CollisionProcessor>;
using StrictCollisionProcessorPointer = Pointer<StrictCollisionProcessor>;

using SimpleKeyPointer = Key*;
using SimpleGameObjectPointer = GameObject*;
using SimplePhysicalObjectPointer = PhysicalObject*;

using GameObjectIteratorPtr = IteratorPointer<GameObjectPointer>;
using RectangleIteratorPtr = IteratorPointer<Rectangle>;
using PhysicalObjectIteratorPtr = IteratorPointer<PhysicalObject>;

template <class PrecessedNodeType> using SimpleVisitorPointer = Pointer<SimpleVisitor<PrecessedNodeType>>;
template <class PrecessedNodeType> using SimpleHierarchicalVisitorPointer = Pointer<SimpleHierarchicalVisitor<PrecessedNodeType>>;

}  // namespace Platformer

#endif  // TYPES_H
