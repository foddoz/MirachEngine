#include "player_script.hpp"

PlayerScript::PlayerScript(const Entity& player, EntityManager* entityManager, PhysicsSystem* physicsSystem)
    : 
    m_playerID(player), 
    m_entityManager(entityManager), 
    m_physicsSystem(physicsSystem)
{
    // Store player entity ID
}

bool PlayerScript::CaptureInput(InputManager& inputManager)
{
    // Poll keyboard/mouse input
    bool quit = inputManager.Update();

    // Update directional and action input flags
    m_moveForward = inputManager.IsActionPressed(InputAction::MoveForward);
    m_moveBackward = inputManager.IsActionPressed(InputAction::MoveBackward);
    m_moveLeft = inputManager.IsActionPressed(InputAction::MoveLeft);
    m_moveRight = inputManager.IsActionPressed(InputAction::MoveRight);
    m_jumpPressed = inputManager.WasActionJustPressed(InputAction::Jump);
    m_spaceDown = inputManager.IsKeyDown(KeyCode::Space);
    m_ctrlDown = inputManager.IsKeyDown(KeyCode::LeftCtrl);
    m_rightMouseDown = inputManager.IsMouseButtonDown(MouseButton::Right);
    m_leftMousePressed = inputManager.WasMouseButtonJustPressed(MouseButton::Left);
    m_eKeyHeld = inputManager.IsActionPressed(InputAction::E_Key);
    m_eKeyPressed = inputManager.WasActionJustPressed(InputAction::E_Key);
    if (inputManager.WasActionJustPressed(InputAction::Quit)) 
    {
        m_exitPressed = true;
        std::cout << "[PlayerScript] ESC (Quit) key pressed!\n";
    }

    m_keyDownR = inputManager.IsKeyDown(KeyCode::R);
    m_keyDownT = inputManager.IsKeyDown(KeyCode::T);
    m_keyDownF = inputManager.IsKeyDown(KeyCode::F);
    m_keyDownC = inputManager.IsKeyDown(KeyCode::C);

    // Update camera orientation based on mouse movement
    Camera& cam = m_entityManager->GetComponent<Camera>(m_playerID);
    cam.Look(inputManager.GetMouseDeltaX(), inputManager.GetMouseDeltaY(), 0.1f);

    // Update camera frustum position for view culling or raycasting
    Transform& tf = m_entityManager->GetComponent<Transform>(m_playerID);
    cam.UpdateFrustumPlanes(tf.GetPosition());

    return quit;
}

bool PlayerScript::IsLMBPressed() const
{
    return m_leftMousePressed;
}