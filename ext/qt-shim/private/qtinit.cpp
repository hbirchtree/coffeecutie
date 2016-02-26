#include <coffee_ext/qt_shim/qtinit.h>

#include <QApplication>

namespace CoffeeExt{

thread_local QApplication* dialog_application = nullptr;

bool QtSystem::Init(int& argc, char** argv)
{
    if(!dialog_application)
    {
        dialog_application = new QApplication(argc,argv);
        return true;
    }
    return false;
}
bool QtSystem::Deinit()
{
    delete dialog_application;
    return true;
}

bool QtSystem::Process(int timeout)
{
    QApplication::processEvents(QEventLoop::AllEvents,timeout);
    return true;
}

int QtSystem::Exec()
{
    return dialog_application->exec();
}

void QtSystem::SetApplicationName(Coffee::cstring name, Coffee::cstring orgname)
{
    QCoreApplication::setApplicationName(name);
    if(orgname)
        QCoreApplication::setOrganizationName(orgname);
}

}
