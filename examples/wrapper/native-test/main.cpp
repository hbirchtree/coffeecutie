#include <coffeeswig.h>

int main()
{
    Coffee::Init();

    Coffee::cDebug("Nice!");

    Coffee::CGLWindow* win = Coffee::Window::CreateWindow(1280,720);
    Coffee::Window::ShowWindow(win);

    while(!Coffee::Window::ShouldClose(win))
    {
        Coffee::Window::Process(win);
    }

    Coffee::Terminate();

    return 0;
}
