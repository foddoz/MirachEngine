#include "input_manager.hpp"

InputManager::InputManager()
    :
    m_keyStates(nullptr),
    m_mouseButtonState(0),
    m_prevMouseButtonState(0),
    m_mouseDeltaX(0.0f),
    m_mouseDeltaY(0.0f)
{

}

bool InputManager::Update()
{
    bool quit = false;

    // Reset mouse movement deltas for this frame
    m_mouseDeltaX = 0.0f;
    m_mouseDeltaY = 0.0f;

    // Save previous keyboard states
    int numKeys = 0;
    KeyStateArray currentKeys = InputBackend::GetKeyboardState(&numKeys);

    // Resize previous key state buffer if needed
    if (m_prevKeyStates.size() != static_cast<size_t>(numKeys)) 
    {
        m_prevKeyStates.resize(numKeys);
    }

    // Copy current keyboard state into previous key state buffer
    for (int i = 0; i < numKeys; ++i) 
    {
        m_prevKeyStates[i] = currentKeys[i];
    }

    // Save current mouse button state to previous
    m_prevMouseButtonState = m_mouseButtonState;

    // Poll input events (mouse motion, quit, button press)
    InputEvent evt;
    while (InputBackend::PollEvent(evt)) 
    {
        // Application quit event
        if (evt.type == InputEventType::Quit) 
        {
            quit = true;
        }

        // Accumulate mouse motion deltas
        if (evt.type == InputEventType::MouseMotion) 
        {
            m_mouseDeltaX += evt.mouseDeltaX;
            m_mouseDeltaY += evt.mouseDeltaY;
        }

        // Update current mouse button state
        if (evt.type == InputEventType::MouseButtonDown || evt.type == InputEventType::MouseButtonUp) 
        {
            m_mouseButtonState = InputBackend::GetMouseButtons();
        }
    }

    // Store current keyboard state pointer for this frame
    m_keyStates = InputBackend::GetKeyboardState(nullptr);

    return quit;
}

bool InputManager::IsKeyDown(KeyCode key) const
{
    // Return true if the key is currently pressed
    return m_keyStates && m_keyStates[static_cast<uint16_t>(key)];
}

bool InputManager::IsActionPressed(InputAction action) const
{
    // Lookup action binding and check if its key is currently down
    auto it = m_actionBindings.find(action);
    return it != m_actionBindings.end() && IsKeyDown(it->second);
}

bool InputManager::WasActionJustPressed(InputAction action) const
{
    // Check if action was not pressed in the previous frame but is pressed now
    auto it = m_actionBindings.find(action);
    if (it != m_actionBindings.end()) 
    {
        uint16_t key = static_cast<uint16_t>(it->second);
        return m_prevKeyStates.size() > key &&
            m_prevKeyStates[key] == 0 &&  // not pressed last frame
            m_keyStates[key] != 0;        // pressed this frame
    }
    return false;
}

bool InputManager::IsMouseButtonDown(MouseButton button) const 
{
    // Check if button is currently down using bitmask
    return (m_mouseButtonState & MouseButtonMask(button)) != 0;
}

bool InputManager::WasMouseButtonJustPressed(MouseButton button) const 
{
    // True if button was not pressed last frame but is pressed now
    uint32_t mask = MouseButtonMask(button);
    return !(m_prevMouseButtonState & mask) && (m_mouseButtonState & mask);
}

void InputManager::MapAction(InputAction action, KeyCode key)
{
    // Bind a high-level action to a physical key
    m_actionBindings[action] = key;
}

float InputManager::GetMouseDeltaX() const 
{ 
    return m_mouseDeltaX; 
}

float InputManager::GetMouseDeltaY() const 
{ 
    return m_mouseDeltaY; 
}
