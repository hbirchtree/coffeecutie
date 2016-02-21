#include <coffee_ext/qt_shim/qtinit.h>

#include <QApplication>

namespace CoffeeExt{

thread_local QApplication* dialog_application = nullptr;

void QtSystem::Init(int& argc, char** argv)
{
    if(!dialog_application)
    {
        dialog_application = new QApplication(argc,argv);
    }
}
void QtSystem::Deinit()
{
    delete dialog_application;
}

void QtSystem::Process(int timeout)
{
    QApplication::processEvents();
}

}