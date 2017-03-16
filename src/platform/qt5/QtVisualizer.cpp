// QtVisualizer.cpp

#include <iostream>

#include <QWidget>
#include <QPainter>
#include <QGraphicsScene>
#include <QKeyEvent>

#include "geometry/Point.h"
#include "platform/Key.h"
#include "platform/Platform.h"
#include "platform/PlatformManager.h"
#include "QtCanvasWidget.h"
#include "QtVisualizer.h"


namespace Platformer
{


struct QtVisualizer::Impl
{
    Impl()
    {
    }

    static Rectangle qrectfToRectangle(QRectF rect);
    static QRectF rectangleToQRectf(Rectangle rect);
    static char fromQtKeyToChar(int qtKeyId);
    static Key::KeyId fromQtKeyToId(int qtKeyId);

    QtCanvasWidget *_canvasPtr;
    QPainter *_painterPtr;
};



QtVisualizer::QtVisualizer()
    : _pimpl(new Impl())
{
    _pimpl->_painterPtr = new QPainter();
    _pimpl->_canvasPtr = new QtCanvasWidget();

    _pimpl->_canvasPtr->onKeyPress = [this](QKeyEvent *eventPtr)
    {
        char ch = Impl::fromQtKeyToChar(eventPtr->key());
        Key::KeyId id = Impl::fromQtKeyToId(eventPtr->key());

        if (ch != 0)
            Platform::instance()->triggerKey(ch, true);
        else if (id != 0)
            Platform::instance()->triggerKey(id, true);
    };

    _pimpl->_canvasPtr->onKeyRelease = [this](QKeyEvent *eventPtr)
    {
        char ch = Impl::fromQtKeyToChar(eventPtr->key());
        Key::KeyId id = Impl::fromQtKeyToId(eventPtr->key());

        if (ch != 0)
            Platform::instance()->triggerKey(ch, false);
        else if (id != 0)
            Platform::instance()->triggerKey(id, false);
    };

    _pimpl->_canvasPtr->show();
}

QtVisualizer::QtVisualizer(QtVisualizer&& /*other*/) = default;
QtVisualizer& QtVisualizer::operator=(QtVisualizer&& /*other*/) = default;

QtVisualizer::~QtVisualizer()
{
}



Rectangle QtVisualizer::getSceneRect() const
{
    return Impl::qrectfToRectangle(_pimpl->_canvasPtr->rect());
}

void QtVisualizer::clear()
{
    _pimpl->_painterPtr->begin(_pimpl->_canvasPtr->getPixmap().get());
    _pimpl->_painterPtr->fillRect(_pimpl->_canvasPtr->rect(), QBrush(Qt::lightGray));
    _pimpl->_painterPtr->end();
}


void QtVisualizer::drawRect(const Rectangle &rect, bool isMovable, bool isStatic, bool isStand)
{
    QPen pen(Qt::black);
    pen.setWidth(isStand ? 2 : 1);

    _pimpl->_painterPtr->begin(_pimpl->_canvasPtr->getPixmap().get());
    _pimpl->_painterPtr->setPen(pen);
    _pimpl->_painterPtr->setBrush(QBrush(isMovable ? Qt::white : Qt::gray));
    _pimpl->_painterPtr->drawRect(Impl::rectangleToQRectf(rect));

    if (isMovable && isStatic)
    {
        _pimpl->_painterPtr->setPen(QPen(Qt::black));
        _pimpl->_painterPtr->drawLine(QPointF(rect.getLeft(), rect.getTop()), QPointF(rect.getRight(), rect.getBottom()));
        _pimpl->_painterPtr->drawLine(QPointF(rect.getLeft(), rect.getBottom()), QPointF(rect.getRight(), rect.getTop()));
    }

    _pimpl->_painterPtr->end();
}


void QtVisualizer::setSceneRect(const Rectangle &rect)
{
    _pimpl->_canvasPtr->resize(static_cast<int>(rect.getWidth()),
                               static_cast<int>(rect.getHeight()));
}


void Platformer::QtVisualizer::refresh()
{
    _pimpl->_canvasPtr->update();
}


Rectangle QtVisualizer::Impl::qrectfToRectangle(QRectF rect)
{
    return Rectangle(Point(rect.x(), rect.y()), Point(rect.width(), rect.height()));
}


QRectF QtVisualizer::Impl::rectangleToQRectf(Rectangle rect)
{
    return QRectF(QPointF(rect.getX(), rect.getY()), QSizeF(rect.getWidth(), rect.getHeight()));
}


char QtVisualizer::Impl::fromQtKeyToChar(int qtKeyId)
{
    if (qtKeyId >= Qt::Key_A && qtKeyId <= Qt::Key_Z)
        return 'a' + static_cast<char>(qtKeyId - Qt::Key_A);

    return 0;
}


Key::KeyId QtVisualizer::Impl::fromQtKeyToId(int qtKeyId)
{
    switch (qtKeyId)
    {
    case Qt::Key_Left:  return Key::Left;
    case Qt::Key_Right: return Key::Right;
    case Qt::Key_Up:    return Key::Up;
    case Qt::Key_Down:  return Key::Down;
    default:            return Key::Unknown;
    };
}



}  // namespace Platformer




