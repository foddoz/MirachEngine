#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "state.h"

/**
 * @brief A generic finite state machine (FSM) class template.
 * This class manages state transitions and execution logic for an owner entity.
 * It supports current, previous, and global states. Global states execute every update,
 * while current states define the main behavior.
 * @tparam EntityType The type of the entity that owns this state machine.
 */
template <class EntityType>
class StateMachine 
{
private:
    EntityType* m_owner;  ///< Pointer to the entity that owns this state machine.
    State<EntityType>* m_currentState = nullptr;  ///< Pointer to the current state.
    State<EntityType>* m_previousState = nullptr; ///< Pointer to the previous state.
    State<EntityType>* m_globalState = nullptr;   ///< Pointer to the global state that executes every update.

public:
    /**
     * @brief Constructor.
     * @param owner Pointer to the owner entity.
     */
    StateMachine(EntityType* owner) 
        : 
        m_owner(owner) 
    {

    }

    /**
     * @brief Calls the Execute() function of both global and current states.
     * This should be called once per frame to update the behavior of the entity.
     */
    void Update() const 
    {
        if (m_globalState) 
        {
            m_globalState->Execute(m_owner);
        }
        if (m_currentState) 
        {
            m_currentState->Execute(m_owner);
        }
    }

    /**
     * @brief Changes the current state to a new one.
     * Calls Exit() on the old state and Enter() on the new one.
     * Also stores the previous state for potential reversion.
     * @param newState Pointer to the new state to transition into.
     */
    void ChangeState(State<EntityType>* newState) 
    {
        if (!newState)
        {
            return;
        }
        m_previousState = m_currentState;
        if (m_currentState) 
        {
            m_currentState->Exit(m_owner);
        }
        m_currentState = newState;
        m_currentState->Enter(m_owner);
    }

    /**
     * @brief Reverts the state machine to the previous state.
     * This calls ChangeState() with the previous state pointer.
     */
    void RevertToPreviousState() 
    {
        ChangeState(m_previousState);
    }

    /**
     * @brief Sets the current state (without triggering Enter()).
     * @param s Pointer to the state to set as current.
     */
    void SetCurrentState(State<EntityType>* s) 
    {
        m_currentState = s;
    }

    /**
     * @brief Sets the global state.
     * Global states execute during every call to Update().
     * @param s Pointer to the state to set as global.
     */
    void SetGlobalState(State<EntityType>* s) 
    {
        m_globalState = s;
    }

    /**
     * @brief Sets the previous state manually.
     * @param s Pointer to the state to set as previous.
     */
    void SetPreviousState(State<EntityType>* s) 
    {
        m_previousState = s;
    }

    /**
     * @brief Gets the current state.
     * @return Pointer to the current state.
     */
    State<EntityType>* GetCurrentState() const 
    {
        return m_currentState;
    }

    /**
     * @brief Gets the previous state.
     * @return Pointer to the previous state.
     */
    State<EntityType>* GetPreviousState() const 
    {
        return m_previousState;
    }

    /**
     * @brief Gets the global state.
     * @return Pointer to the global state.
     */
    State<EntityType>* GetGlobalState() const 
    {
        return m_globalState;
    }

    /**
     * @brief Checks if the current state matches a given state type.
     * Uses RTTI (typeid) to compare state types.
     * @param st Reference to the state to compare with.
     * @return true if the current state's type matches, false otherwise.
     */
    bool IsInState(const State<EntityType>& st) const 
    {
        return typeid(*m_currentState) == typeid(st);
    }
};

#endif // STATEMACHINE_H
