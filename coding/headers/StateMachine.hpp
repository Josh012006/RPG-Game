#pragma once

#include <memory>
#include <stack> // To manage our states: the latest is on top

#include "State.hpp"


namespace Josh {
    typedef std::unique_ptr<State> StateRef;

    class StateMachine {
    public:
        StateMachine() {}
        ~StateMachine() {}

        // Used to add a state to the stack
        void AddState(StateRef newState, bool isReplacing = true);
        // To manually remove the top State
        void RemoveState();

        //
        void ProcessStateChanges();

        // Getting the top level state in the stack
        StateRef& GetActiveState();

        // Checking if the state machine is empty
        bool IsEmpty();

    private:
        std::stack<StateRef> _states; // The stack containing our states
        StateRef _newState; // Variable representing a new state we want to add

        bool _isRemoving; // Keep track if we are removing a state. Used to process the state changes;
        bool _isAdding; // Keep track if we are adding a state. Used to process the state changes;
        bool _isReplacing; // Keep track if we are replacing a state. Used to process the state changes;
    };
}
