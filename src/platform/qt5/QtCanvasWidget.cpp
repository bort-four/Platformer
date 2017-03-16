// QtCanvasWidget.cpp

#include <iostream>

#include <QPainter>
#include <QDebug>

#include "QtCanvasWidget.h"


namespace Platformer
{


struct QtCanvasWidget::Impl
{
    Impl()
    {
    }

    PixmapPtr _pixmapPtr;
};



QtCanvasWidget::QtCanvasWidget()
    : QWidget()
    , _pimpl(new Impl())
{
    _pimpl->_pixmapPtr.reset(new QPixmap(size()));
}

QtCanvasWidget::~QtCanvasWidget()
{
}


QtCanvasWidget::PixmapPtr QtCanvasWidget::getPixmap() const
{
    return _pimpl->_pixmapPtr;
}


void Platformer::QtCanvasWidget::keyPressEvent(QKeyEvent *eventPtr)
{
    QWidget::keyPressEvent(eventPtr);

    if (onKeyPress != nullptr)
        onKeyPress(eventPtr);
}

void Platformer::QtCanvasWidget::keyReleaseEvent(QKeyEvent *eventPtr)
{
    QWidget::keyReleaseEvent(eventPtr);

    if (onKeyRelease != nullptr)
        onKeyRelease(eventPtr);
}

void Platformer::QtCanvasWidget::resizeEvent(QResizeEvent *eventPtr)
{
    QWidget::resizeEvent(eventPtr);

    QPixmap *newPixmap = new QPixmap(size());
    QPainter painter;
    painter.begin(newPixmap);
    painter.drawPixmap(rect(), *_pimpl->_pixmapPtr);
    painter.end();
    _pimpl->_pixmapPtr.reset(newPixmap);

    if (onResize != nullptr)
        onResize();
}

void Platformer::QtCanvasWidget::paintEvent(QPaintEvent *eventPtr)
{
    QWidget::paintEvent(eventPtr);

    QPainter painter;
    painter.begin(this);
    painter.drawPixmap(rect(), *_pimpl->_pixmapPtr);
    painter.end();
}


}  // namespace Platformer


