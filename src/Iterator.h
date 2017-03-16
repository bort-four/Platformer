// Iterator.h

#ifndef ITERATOR_H
#define ITERATOR_H

#include <memory>
#include <map>
#include <stack>
#include <functional>
#include <iostream>


template <class Type> class Iterator;
template <class Type> using IteratorPointer = std::shared_ptr<Iterator<Type>>;


template <class IteratorPtrType, class funcType>
void forEach(IteratorPtrType iterPtr, funcType func);

template <class ValueType>
size_t getCount(IteratorPointer<ValueType> iterPtr);

template <class STLMapType>
IteratorPointer<typename STLMapType::mapped_type>
createMapValueIterator(const STLMapType &map);

template <class STLContainerType>
IteratorPointer<typename STLContainerType::value_type>
createContainerIterator(const STLContainerType &container);

template <class ValueType, class PredicateType>
IteratorPointer<ValueType>
createConditionIterator(IteratorPointer<ValueType> baseIterPtr,
                        const PredicateType &predicate);

template <class ValueType, class PredicateType>
IteratorPointer<ValueType> find(IteratorPointer<ValueType> iterPtr,
                                const PredicateType &predicate);


template <class ValueType>
class Iterator
{
public:
	using value_type = ValueType;

	Iterator(Iterator&& other) = default;
	virtual Iterator& operator=(Iterator&& other) = default;
	virtual ~Iterator() = default;
	
    virtual void first() = 0;
	virtual void next() = 0;
	virtual bool isDone() const = 0;
	
	virtual const ValueType &current() const = 0;
	
protected:
	explicit Iterator() = default;
};


template <class ValueType>
class EmptyIterator : public Iterator<ValueType>
{
public:
    explicit EmptyIterator() = default;
    EmptyIterator(EmptyIterator&& other) = default;
    virtual EmptyIterator& operator=(EmptyIterator&& other) = default;
    virtual ~EmptyIterator() = default;

    virtual void first() override {}
    virtual void next() override {}
    virtual bool isDone() const override { return true; }

    virtual const ValueType &current() const override
    {
        throw std::logic_error("EmptyIterator::current()");
    }
};


template <class ValueType>
class SingleValueIterator : public Iterator<ValueType>
{
public:
    explicit SingleValueIterator(const ValueType &value)
        : Iterator<ValueType>()
        , _value(value)
    {
    }

    SingleValueIterator(SingleValueIterator&& other) = default;
    virtual SingleValueIterator& operator=(SingleValueIterator&& other) = default;
    virtual ~SingleValueIterator() = default;

    virtual void first() override { _isDone = false; }
    virtual void next() override { _isDone = true; }
    virtual bool isDone() const override { return _isDone; }

    virtual const ValueType &current() const override
    {
        return _value;
    }

private:
    ValueType _value;
    bool _isDone = true;
};



template <class ValueType/*, class PredicateType*/>
class ConditionIterator : public Iterator<ValueType>
{
public:
    using PredicateType = std::function<bool(const ValueType&)>;

	explicit ConditionIterator(IteratorPointer<ValueType> baseIterPtr,
							   const PredicateType &predicate)
		: Iterator<ValueType>()
		, _baseIterPtr(baseIterPtr)
		, _predicate(predicate)
	{
	}
	
	ConditionIterator(ConditionIterator&& other) = default;
	virtual ConditionIterator& operator=(ConditionIterator&& other) = default;
	virtual ~ConditionIterator() = default;
	
    virtual void first() override
	{
        _baseIterPtr->first();
		move();
	}
	
	virtual void next() override
	{
		_baseIterPtr->next();
		move();
	}
	
	virtual bool isDone() const override { return _baseIterPtr->isDone(); }
	
	virtual const ValueType &current() const override
	{
		return _baseIterPtr->current();
	}


protected:
	void move()
	{
		for ( ; !_baseIterPtr->isDone() && !_predicate(_baseIterPtr->current());
			_baseIterPtr->next())
			;
	}

private:
	IteratorPointer<ValueType> _baseIterPtr;
	PredicateType _predicate;
};



template <class ValueType, class STLIteratorType>
class STLRangeIterator : public Iterator<ValueType>
{
public:
	STLRangeIterator(const STLIteratorType &begin, const STLIteratorType &end)
		: Iterator<ValueType>()
		, _begin(begin)
		, _end(end)
		, _it(begin)
	{
	}
	
	STLRangeIterator(STLRangeIterator&& other) = default;
	virtual STLRangeIterator& operator=(STLRangeIterator&& other) = default;
	virtual ~STLRangeIterator() = default;
	
    virtual void first() override { _it = _begin; }
	
	virtual void next() override
	{
		if (!isDone())
			++_it;
	}
	
	virtual bool isDone() const override { return _it == _end; }
	
	virtual const ValueType &current() const
	{
		return *_it;
	}
	
private:
	STLIteratorType _begin, _end, _it;
};



template <class ValueType, class KeyType>
class MapValueIterator : public Iterator<ValueType>
{
public:
    using Map = std::map<KeyType, ValueType>;
    using BaseIterator = STLRangeIterator<typename Map::value_type,
                                          typename Map::const_iterator>;

    MapValueIterator(const std::map<KeyType, ValueType> &map)
        : _baseIter(map.cbegin(), map.cend())
    {
    }

    MapValueIterator(MapValueIterator&& other) = default;
    virtual MapValueIterator& operator=(MapValueIterator&& other) = default;
    virtual ~MapValueIterator() = default;

    virtual void first() override { _baseIter.first(); }
    virtual void next() override { _baseIter.next(); }
    virtual bool isDone() const override { return _baseIter.isDone(); }

    virtual const ValueType &current() const
    {
        return _baseIter.current().second;
    }

private:
    BaseIterator _baseIter;
};


enum TreeIteratorOrder {Preorder, Postorder};

template <class ValueType, TreeIteratorOrder order = Preorder>
class TreeIterator : public Iterator<ValueType>
{
public:
    using SubIteratorGetter = std::function<IteratorPointer<ValueType>(const ValueType&)>;

    TreeIterator(const ValueType &rootObject,
                 SubIteratorGetter subObjectGetter)
        : Iterator<ValueType>()
        , _rootObject(rootObject)
        , _subObjectGetter(subObjectGetter)
    {
    }

    TreeIterator(TreeIterator&& other) = default;
    virtual TreeIterator& operator=(TreeIterator&& other) = default;
    virtual ~TreeIterator() = default;

    virtual void first() override
    {
        for ( ; !_layerStack.empty(); )
            _layerStack.pop();

        _layerStack.push(IteratorPointer<ValueType>(
                             new SingleValueIterator<ValueType>(_rootObject)));

        if (order == Postorder)
        {
            for (_layerStack.top()->first(); !_layerStack.top()->isDone(); )
                _layerStack.push(_subObjectGetter(_layerStack.top()->current()));

            _layerStack.pop();
        }
    }

    virtual void next() override
    {
        if (_layerStack.empty())
            throw std::logic_error("TreeIterator::next: iterator is done");

        switch (order)
        {
        case Preorder:
        {
            _layerStack.push(_subObjectGetter(_layerStack.top()->current()));

            for ( ; !_layerStack.empty() && _layerStack.top()->isDone(); )
            {
                _layerStack.pop();

                if (!_layerStack.empty())
                    _layerStack.top()->next();
            }

            break;
        }
        case Postorder:
        {
            _layerStack.top()->next();

            for ( ; !_layerStack.top()->isDone(); )
                _layerStack.push(_subObjectGetter(_layerStack.top()->current()));

            _layerStack.pop();
            break;
        }
        default:
            throw std::logic_error("TreeIterator::next: invalid order type");
        }
    }

    virtual bool isDone() const override
    {
        return _layerStack.empty();
    }

    virtual const ValueType &current() const
    {
        return _layerStack.top()->current();
    }

private:
    ValueType _rootObject;
    SubIteratorGetter _subObjectGetter;
    std::stack<IteratorPointer<ValueType> > _layerStack;
};



template<class ValueType>
class ParameterizedRange
{
public:
    class iterator : public std::iterator<
            std::input_iterator_tag,
            ValueType,
            ValueType,
            const ValueType*,
            ValueType>
    {
    public:
        explicit iterator(ValueType num = 0, ValueType step = 1)
            : _pos(num)
            , _step(step)
        {
        }

        iterator& operator++()
        {
            _pos += _step; return *this;
        }

        iterator operator++(int)
        {
            iterator result = *this;
            ++(*this);
            return result;
        }

        bool operator==(const iterator &other) const
        {
            return _pos == other._pos;
        }

        bool operator!=(const iterator &other) const
        {
            return !(*this == other);
        }

        ValueType operator*() const
        {
            return _pos;
        }
    private:
        ValueType _pos, _step;
    };

    ParameterizedRange(ValueType begin, ValueType end, ValueType step)
        : _begin(begin), _end(end), _step(step)
    {
        if (step == 0 || (end - begin) * step < 0)
            throw std::logic_error("Range: step is invalid");
    }

    ParameterizedRange(ValueType begin, ValueType end)
        : ParameterizedRange(begin, end, (begin <= end) ? 1 : -1)
    {
    }

    ParameterizedRange(ValueType end)
        : ParameterizedRange(0, end)
    {
    }

//    template<class ValueType2>
//    ParameterizedRange(ValueType2 end)
//        : ParameterizedRange(0, static_cast<ValueType>(end))
//    {
//    }

    iterator begin()
    {
        return iterator(_begin, _step);
    }

    iterator end()
    {
        return iterator(_end, 0);
    }

private:
    ValueType _begin, _end, _step;
};

using Range = ParameterizedRange<long>;


// Implementation

template <class STLMapType>
IteratorPointer<typename STLMapType::mapped_type>
createMapValueIterator(const STLMapType &map)
{
    auto ptr = new MapValueIterator<typename STLMapType::mapped_type,
                                    typename STLMapType::key_type>(map);

    return IteratorPointer<typename STLMapType::mapped_type>(ptr);
}


template <class STLContainerType>
IteratorPointer<typename STLContainerType::value_type>
createContainerIterator(const STLContainerType &container)
{	
	auto ptr = new STLRangeIterator<typename STLContainerType::value_type,
									typename STLContainerType::const_iterator>(
										container.cbegin(), container.cend());
	
	return IteratorPointer<typename STLContainerType::value_type>(ptr);
}


template <class ValueType, class PredicateType>
IteratorPointer<ValueType>
createConditionIterator(IteratorPointer<ValueType> baseIterPtr,
						const PredicateType &predicate)
{	
    auto ptr = new ConditionIterator<ValueType/*, PredicateType*/>
									(baseIterPtr, predicate);
	
	return IteratorPointer<ValueType>(ptr);
}


template <class ValueType, class PredicateType>
IteratorPointer<ValueType> find(IteratorPointer<ValueType> iterPtr,
                                const PredicateType &predicate)
{
    for (iterPtr->first(); !iterPtr->isDone(); iterPtr->next())
        if (predicate(iterPtr->current()))
            return iterPtr;

    return iterPtr;
}


template <class IteratorPtrType, class funcType>
void forEach(IteratorPtrType iterPtr, funcType func)
{
    for (iterPtr->first(); !iterPtr->isDone(); iterPtr->next())
		func(iterPtr->current());
}


template <class ValueType>
size_t getCount(IteratorPointer<ValueType> iterPtr)
{
	size_t count = 0;
	
	forEach(iterPtr, [&count](const ValueType &)
	{
		++count;
	});
	
	return count;
}



#endif // ITERATOR_H





//~ template <class STLContainerType>
//~ STLRangeIterator<typename STLContainerType::value_type,
				 //~ typename STLContainerType::const_iterator>
//~ createContainerIterator(const STLContainerType &container)
//~ {
	//~ return STLRangeIterator<typename STLContainerType::value_type,
							//~ typename STLContainerType::const_iterator>(
								//~ container.cbegin(), container.cend());
//~ }
