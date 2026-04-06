#ifndef TIME_HPP
#define TIME_HPP

#include <SDL3/SDL_timer.h>

namespace Time 
{
    inline float GetTimeSeconds() 
    {
        return SDL_GetTicks() / 1000.0f;
    }
}

#endif // TIME_HPP