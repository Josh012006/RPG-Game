#include "headers/StateMachine.hpp"

namespace Josh {
    void StateMachine::AddState(Josh::StateRef newState, bool isReplacing) {
        this->_isAdding = true; // Specify we are effectively adding a state
        this->_isReplacing = isReplacing; // Specify if we are replacing an existing state or not based on the second
                                            // argument of the method

        this->_newState = std::move(newState); // Specifying the newState
    }

    void StateMachine::RemoveState() {
        this->_isRemoving = true; // Specify we are removing a state
    }

    void StateMachine::ProcessStateChanges() {
        // We are verifying if we want to remove a state and if the states stack is not empty
        if(this->_isRemoving && !this->_states.empty()) {
            // Then we remove the top state
            this->_states.pop();

            // We verify if the stack is not empty
            if(!this->_states.empty()) {
                // We can now go back to the top state or to be precise resume the previous state of the game
                this->_states.top()->Resume();
            }

            // We ensure we are showing that the removing operation is done in other not to have all states removed
            this->_isRemoving = false;
        }

        // Managing the adding of states
        if(this->_isAdding) {
            // We want to see if the states stack is not empty in other to verify if we must
            // replace the last state
            if(!this->_states.empty()) {
                // We check if isReplacing is true
                if(this->_isReplacing) {
                    // We remove the last state then add the new state
                    this->_states.pop();
                }
                else {
                    // If we are not replacing we just pause the current state
                    this->_states.top()->Pause();
                }
            }

            // We then add the new state
            this->_states.push(std::move(this->_newState));
            // We then init the new added state
            this->_states.top()->Init();

            // Then we make sure to specify the adding operation as done
            this->_isAdding = false;
        }
    }

    StateRef& StateMachine::GetActiveState() {
        // Return the top state
        return this->_states.top();
    }

    bool StateMachine::IsEmpty() {
        return this->_states.empty();
    }
}
