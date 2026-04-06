#pragma once

#include <cstdint>
#include <SDL3/SDL.h> // Not good, should refactor in the future.
#include <vector>
#include <functional>

/**
 * @enum KeyCode
 * @brief Enum representing abstracted keyboard key codes (mapped from SDL scancodes).
 */
enum class KeyCode : uint16_t 
{
    Unknown = 0,   ///< Unknown or unrecognized key
    W = 26,        ///< Forward movement key (typically 'W')
    A = 4,         ///< Left movement key (typically 'A')
    S = 22,        ///< Backward movement key (typically 'S')
    D = 7,         ///< Right movement key (typically 'D')
    Space = 44,     ///< Jump or action key (spacebar)
    E = 8,
    Escape = 41,    ///< Escape key (ESC)
    LeftCtrl = 224,
    R = 21,
    T = 23,
    F = 9,
    C = 6,
    BackSlash = 49

    // Add more as needed
};

/**
 * @enum MouseButton
 * @brief Enum representing mouse buttons (abstracted from SDL).
 */
enum class MouseButton 
{
    Left = 1,     ///< Left mouse button
    Right = 3,    ///< Right mouse button
    Middle = 2    ///< Middle (wheel) mouse button
};

/**
 * @enum InputEventType
 * @brief Enum representing the type of input event polled from SDL.
 */
enum class InputEventType 
{
    Quit,             ///< Application quit event
    MouseMotion,      ///< Mouse movement event
    MouseButtonDown,  ///< Mouse button press
    MouseButtonUp,    ///< Mouse button release
    Unknown           ///< Unrecognized or unsupported event
};

/**
 * @struct InputEvent
 * @brief Structure holding abstracted input event data for use in the engine.
 */
struct InputEvent 
{
    InputEventType type;  ///< Type of input event
    float mouseDeltaX; ///< Change in mouse X since last event
    float mouseDeltaY; ///< Change in mouse Y since last event

    InputEvent()
        :
        type(InputEventType::Unknown),
        mouseDeltaX(0.f),
        mouseDeltaY(0.f)
    {

    }
};

/**
 * @typedef KeyStateArray
 * @brief Raw pointer to SDL keyboard state array.
 */
using KeyStateArray = const uint8_t*;

/**
 * @brief Utility to compute the SDL mouse button bitmask for a given button.
 * @param button The mouse button.
 * @return Bitmask representing that button.
 */
inline constexpr uint32_t MouseButtonMask(MouseButton button) 
{
    return 1u << (static_cast<int>(button) - 1);
}

/**
 * @namespace InputBackend
 * @brief Backend namespace for platform-specific input functionality (SDL abstraction).
 */
namespace InputBackend 
{

    extern std::vector<std::function<bool(const SDL_Event* const)>> rawInputCallbacks;

    /**
     * @brief Initializes the input backend.
     * Must be called before polling events.
     */
    void Init();

    /**
     * @brief Polls a single input event and fills the output event structure.
     * @param outEvent Reference to an InputEvent structure to populate.
     * @return True if an event was polled; false if the event queue is empty.
     */
    bool PollEvent(InputEvent& outEvent);

    /**
     * @brief Returns the current state of all keyboard keys.
     * @param numKeys Pointer to int to receive the number of keys available.
     * @return Pointer to an array of key states (1 = pressed, 0 = not pressed).
     */
    KeyStateArray GetKeyboardState(int* numKeys);

    /**
     * @brief Returns the current state of all mouse buttons.
     * @return Bitmask representing the mouse buttons currently pressed.
     */
    uint32_t GetMouseButtons();
}
