#include "include/cef_app.h"

int main(int argc, char** argv)
{
    CefMainArgs cef_arg(argc,argv);
    CefExecuteProcess(cef_arg,nullptr,nullptr);
    return 0;
}
