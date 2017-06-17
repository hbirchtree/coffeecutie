#include <coffeeswig.h>

int main()
{
    Init();

    Coffee::cDebug("Nice!");

    SDL2Renderer* win = CreateWindow({});
    ShowWindow(win);

    while(!ShouldClose(win))
    {
        Process(win);
    }

    Terminate();

    return 0;
}
