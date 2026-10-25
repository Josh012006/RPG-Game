#pragma once

#include <memory>
#include <vector>

#include "engine/State.hpp"

namespace Oathbound {

    using StateRef = std::unique_ptr<State>;

    // A stack of states. Changes requested during a frame are queued and applied at the start
    // of the next one (ApplyPending), so a state can never be destroyed while it is running.
    // The old version kept ONE pending state, and its flags were uninitialised.
    class StateMachine {
    public:
        void Push(StateRef state);     // keeps the current state underneath (paused)
        void Replace(StateRef state);  // swaps the current state
        void Pop();                    // returns to the state underneath

        void ApplyPending();
        bool Empty() const { return _stack.empty(); }
        State& Active() { return *_stack.back(); }

        // Draws the top state, preceded by the states under it while the ones above are overlays.
        void DrawAll();

    private:
        enum class Op { Push, Replace, Pop };
        struct Command {
            Op op;
            StateRef state;
        };

        std::vector<StateRef> _stack;
        std::vector<Command> _pending;
    };
}
