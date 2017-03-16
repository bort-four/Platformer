// Key.h

#ifndef KEY_H
#define KEY_H

#include <memory>

#include "Iterator.h"


namespace Platformer
{


class Key
{
public:
    enum KeyId
    {
        Unknown = 0,
        Right,
        Left,
        Up,
        Down
    };

    using KeyIdIterator = IteratorPointer<KeyId>;
    using CharIterator = IteratorPointer<char>;
    using EventHandler = std::function<void()>;

    Key(KeyId id);
    Key(char ch);

    template <class ...Args>
    Key(KeyId id, Args ...args);

    template <class ...Args>
    Key(char ch, Args ...args);

    Key(Key&& other);
    virtual Key& operator=(Key&& other);
    virtual ~Key();

    KeyIdIterator getKeyIdIterator() const;
    CharIterator getCharIterator() const;
    bool isPressed() const;

    void addKeyId(KeyId id);
    void addChar(char ch);
    void setIsPressed(bool isPresed);

    EventHandler onPress;
    EventHandler onRelease;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};



template<class ...Args>
Key::Key(KeyId id, Args ...args)
    : Key(args...)
{
    addKeyId(id);
}


template<class ...Args>
Key::Key(char ch, Args ...args)
    : Key(args...)
{
    addChar(ch);
}



}  // namespace Platformer

#endif  // KEY_H
