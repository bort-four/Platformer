// QtApplication.h

#ifndef QTAPPLICATION_H
#define QTAPPLICATION_H

#include <memory>

#include <QApplication>


namespace Platformer
{


class QtApplication : public QApplication
{
    Q_OBJECT

public:
    QtApplication(int &argc, char *argv[]);
    virtual ~QtApplication();

    virtual bool notify(QObject *obj, QEvent *event) override;

    static QtApplication *instance();
};


}  // namespace Platformer

#endif  // QTAPPLICATION_H
