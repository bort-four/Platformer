// Key.cpp

#include <vector>

#include "platform/Platform.h"
#include "platform/PlatformManager.h"
#include "Key.h"


namespace Platformer
{


struct Key::Impl
{
    Impl()
    {
    }

    bool _isPressed = false;
    std::vector<KeyId> _keyIdList;
    std::vector<char> _charList;
};



Key::Key(KeyId id)
    : _pimpl(new Impl())
{
    addKeyId(id);
}


Key::Key(char ch)
    : _pimpl(new Impl())
{
    addChar(ch);
}


Key::Key(Key&& /*other*/) = default;
Key& Key::operator=(Key&& /*other*/) = default;

Key::~Key()
{
    Platform::instance()->unregisterKey(this);
}


Key::KeyIdIterator Key::getKeyIdIterator() const
{
    return createContainerIterator(_pimpl->_keyIdList);
}


Key::CharIterator Key::getCharIterator() const
{
    return createContainerIterator(_pimpl->_charList);
}


bool Key::isPressed() const
{
    return _pimpl->_isPressed;
}



void Key::addKeyId(Key::KeyId id)
{
    _pimpl->_keyIdList.push_back(id);
    Platform::instance()->registerKey(this, id);
}


void Key::addChar(char ch)
{
    _pimpl->_charList.push_back(ch);
    Platform::instance()->registerKey(this, ch);
}


void Key::setIsPressed(bool isPresed)
{
    bool oldIsPressed = _pimpl->_isPressed;
    _pimpl->_isPressed = isPresed;

    if (!oldIsPressed && isPresed && onPress != nullptr)
        onPress();
    else if (oldIsPressed && !isPresed && onRelease != nullptr)
        onRelease();
}


}  // namespace Platformer
