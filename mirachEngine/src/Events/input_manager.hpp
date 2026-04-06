#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "input.hpp"
#include <unordered_map>
#include <vector>

/**
 * @enum InputAction
 * @brief High-level input actions mapped to specific key codes.
 */
enum class InputAction 
{
    MoveForward,   ///< Move player forward
    MoveBackward,  ///< Move player backward
    MoveLeft,      ///< Move player left
    MoveRight,     ///< Move player right
    Jump,          ///< Make the player jump
    Quit,          ///< Trigger application quit
    ObjectLaunch,  ///< Launch a held object
    ObjectGrab,    ///< Grab or pick up an object
    E_Key,
    DebugToggle
};

/**
 * @class InputManager
 * @brief Manages input actions, key states, and mouse interactions at a high level.
 *
 * The InputManager maps abstract InputActions to physical key codes and tracks
 * key/mouse states across frames to support features like "just pressed" detection.
 */
class InputManager 
{
public:

    InputManager();

    /**
     * @brief Polls and updates internal input state for the current frame.
     * @return True if a quit event was detected, false otherwise.
     */
    bool Update();

    /**
     * @brief Checks if a specific key is currently held down.
     * @param key The key code to query.
     * @return True if the key is down.
     */
    bool IsKeyDown(KeyCode key) const;

    /**
     * @brief Checks if an action's mapped key is currently held down.
     * @param action The abstract input action.
     * @return True if the action's key is pressed.
     */
    bool IsActionPressed(InputAction action) const;

    /**
     * @brief Checks if an action's mapped key was just pressed this frame.
     * @param action The abstract input action.
     * @return True if the action was just triggered.
     */
    bool WasActionJustPressed(InputAction action) const;

    /**
     * @brief Checks if a mouse button is currently held down.
     * @param button The mouse button to query.
     * @return True if the button is down.
     */
    bool IsMouseButtonDown(MouseButton button) const;

    /**
     * @brief Checks if a mouse button was just pressed this frame.
     * @param button The mouse button to query.
     * @return True if the button was just pressed.
     */
    bool WasMouseButtonJustPressed(MouseButton button) const;

    /**
     * @brief Gets the horizontal mouse movement delta from this frame.
     * @return The X delta.
     */
    float GetMouseDeltaX() const;

    /**
     * @brief Gets the vertical mouse movement delta from this frame.
     * @return The Y delta.
     */
    float GetMouseDeltaY() const;

    /**
     * @brief Maps a high-level InputAction to a specific KeyCode.
     * @param action The logical action.
     * @param key The key that triggers it.
     */
    void MapAction(InputAction action, KeyCode key);

private:
    const uint8_t* m_keyStates;       ///< Pointer to current frame's key state array.
    std::vector<uint8_t> m_prevKeyStates;       ///< Copy of previous frame's key states.

    uint32_t m_mouseButtonState;            ///< Mouse buttons currently held.
    uint32_t m_prevMouseButtonState;        ///< Mouse buttons from previous frame.

    float m_mouseDeltaX;                 ///< X-axis mouse delta this frame.
    float m_mouseDeltaY;                 ///< Y-axis mouse delta this frame.

    std::unordered_map<InputAction, KeyCode> m_actionBindings; ///< Maps actions to key codes.
};

#endif // !INPUT_MANAGER_HPP
