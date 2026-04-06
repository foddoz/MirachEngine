#pragma once
#ifndef __STATE_H
#define __STATE_H

/**
 * @brief Abstract base class representing a state in a finite state machine (FSM).
 * This class defines the interface for states that can be used within a StateMachine.
 * @tparam T The type of the owner entity using this state.
 */
template <typename T>
class State
{
public:
    /**
     * @brief Virtual destructor to ensure proper cleanup of derived states.
     */
    virtual ~State() {}

    /**
     * @brief Called when the state is entered.
     * This method is used to perform any initialization or setup for the state.
     * @param owner Pointer to the owner entity.
     */
    virtual void Enter(T* owner) = 0;

    /**
     * @brief Called every frame to update the state.
     * This method should implement the main behavior of the state.
     * @param owner Pointer to the owner entity.
     */
    virtual void Execute(T* owner) = 0;

    /**
     * @brief Called when the state is exited.
     * This method should handle cleanup or transition logic before leaving the state.
     * @param owner Pointer to the owner entity.
     */
    virtual void Exit(T* owner) = 0;
};

#endif // __STATE_H
