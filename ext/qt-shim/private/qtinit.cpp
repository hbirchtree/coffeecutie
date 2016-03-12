#include <coffee_ext/qt_shim/qtinit.h>

#include <coffee/core/CProfiling>
#include <QApplication>

namespace CoffeeExt{

thread_local QApplication* dialog_application = nullptr;

bool QtSystem::Init(int& argc, char** argv)
{
    Coffee::Profiler::PushContext("Qt Init");
    if(!dialog_application)
    {
        dialog_application = new QApplication(argc,argv);
        Coffee::Profiler::Profile("QApplication startup");
        Coffee::Profiler::PopContext();
        return true;
    }
    Coffee::Profiler::PopContext();
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

Coffee::CString QtSystem::ApplicationName()
{
    return QCoreApplication::applicationName().toStdString();
}

Coffee::CString QtSystem::OrganizationName()
{
    return QCoreApplication::organizationName().toStdString();
}

void QtSystem::SetApplicationName(Coffee::cstring name, Coffee::cstring orgname)
{
    QCoreApplication::setApplicationName(name);
    if(orgname)
        QCoreApplication::setOrganizationName(orgname);
}

}
