#include <platforms/base/power.h>

#if defined(COFFEE_USE_SDL2)
#include <SDL_power.h>

namespace Coffee{
namespace Environment{

bool _SDLPowerInfo::IsPowered()
{
    SDL_PowerState stat = SDL_GetPowerInfo(nullptr,nullptr);
    return stat==SDL_POWERSTATE_CHARGING
            || stat==SDL_POWERSTATE_CHARGED
            || stat==SDL_POWERSTATE_NO_BATTERY;
}
bool _SDLPowerInfo::IsCharging()
{
    return SDL_GetPowerInfo(nullptr,nullptr)==SDL_POWERSTATE_CHARGING;
}
bool _SDLPowerInfo::HasBattery()
{
    SDL_PowerState state = SDL_GetPowerInfo(nullptr,nullptr);
    return state!=SDL_POWERSTATE_NO_BATTERY &&
            state!=SDL_POWERSTATE_UNKNOWN;
}

uint16 _SDLPowerInfo::BatteryPercentage()
{
    int prct;
    SDL_GetPowerInfo(nullptr,&prct);
    return (prct>=0) ? prct : 0;
}
uint32 _SDLPowerInfo::BatteryLifetime()
{
    int time;
    SDL_GetPowerInfo(&time,nullptr);
    return (time>=0) ? time/60 : 0;
}

}
}
#endif
