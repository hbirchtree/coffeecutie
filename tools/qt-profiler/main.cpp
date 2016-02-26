#include <coffee/core/CApplication>
#include <coffee/core/CMD>
#include <coffee_ext/qt_shim/qtinit.h>

#include <QApplication>
#include "profilingviewer.h"

using namespace Coffee;

int32 coffee_main(int32 argc,cstring_w* argv)
{
    auto qt = SubsystemWrapper<CoffeeExt::QtSystem>();
    C_UNUSED(qt);

    CoffeeExt::QtSystem::SetApplicationName("Coffee Profiler");

    ProfilingViewer viewer;
    viewer.show();

    return CoffeeExt::QtSystem::Exec();
}

COFFEE_APPLICATION_MAIN(coffee_main)
