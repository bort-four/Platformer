// QtCanvasWidget.h

#ifndef QTCANVASWIDGET_H
#define QTCANVASWIDGET_H

#include <memory>

#include <QWidget>
#include <QPixmap>


namespace Platformer
{


class QtCanvasWidget : public QWidget
{
    Q_OBJECT

public:
    using ResizeEventHandler = std::function<void(void)>;
    using KeyEventHandler = std::function<void(QKeyEvent *)>;
    using PixmapPtr = std::shared_ptr<QPixmap>;

    QtCanvasWidget();
    virtual ~QtCanvasWidget();

    PixmapPtr getPixmap() const;

    ResizeEventHandler onResize;
    KeyEventHandler onKeyPress;
    KeyEventHandler onKeyRelease;

protected:
    virtual void keyPressEvent(QKeyEvent *) override;
    virtual void keyReleaseEvent(QKeyEvent *) override;
    virtual void resizeEvent(QResizeEvent *) override;
    virtual void paintEvent(QPaintEvent *) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // QTCANVASWIDGET_H
