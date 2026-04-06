#include "input.hpp"
#include <SDL3/SDL.h>

namespace InputBackend 
{

    std::vector<std::function<bool(const SDL_Event* const)>> rawInputCallbacks;

    // Initializes the SDL event subsystem for input handling
    void Init() 
    {
        SDL_InitSubSystem(SDL_INIT_EVENTS);
    }

    // Polls one SDL event and translates it into an InputEvent
    bool PollEvent(InputEvent& outEvent) 
    {
        SDL_Event e;

        // If no events are in the queue, return false
        if (!SDL_PollEvent(&e))
        {
            return false;
        }

        for (auto& callback : rawInputCallbacks)
        {
            callback(&e);
        }

        // Translate SDL event type into abstracted InputEventType
        switch (e.type) 
        {
        case SDL_EVENT_QUIT:
            outEvent.type = InputEventType::Quit;
            return true;

        case SDL_EVENT_MOUSE_MOTION:
            outEvent.type = InputEventType::MouseMotion;
            outEvent.mouseDeltaX = static_cast<float>(e.motion.xrel); // Relative X movement
            outEvent.mouseDeltaY = static_cast<float>(e.motion.yrel); // Relative Y movement
            return true;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            outEvent.type = InputEventType::MouseButtonDown;
            return true;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            outEvent.type = InputEventType::MouseButtonUp;
            return true;

        default:
            outEvent.type = InputEventType::Unknown;
            return true;
        }
    }

    // Returns a pointer to the current SDL keyboard state array
    KeyStateArray GetKeyboardState(int* numKeys) 
    {
        return reinterpret_cast<KeyStateArray>(SDL_GetKeyboardState(numKeys));
    }

    // Returns the current bitmask of pressed mouse buttons
    uint32_t GetMouseButtons() 
    {
        return SDL_GetMouseState(nullptr, nullptr); // Position not needed
    }

} // namespace InputBackend
