// QtApplication.cpp

#include "Types.h"
#include "platform/Platform.h"
#include "platform/PlatformManager.h"
#include "QtApplication.h"


namespace Platformer
{


QtApplication::QtApplication(int &argc, char *argv[])
    : QApplication(argc, argv)
{
}

QtApplication::~QtApplication()
{
}


bool QtApplication::notify(QObject *obj, QEvent *event)
try
{
    return QApplication::notify(obj, event);
}
catch (const std::exception &error)
{
    Platform::instance()->showError(error.what());
    return false;
}


QtApplication *QtApplication::instance()
{
    return dynamic_cast<QtApplication *>(QApplication::instance());
}


}  // namespace Platformer
