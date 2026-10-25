#include "engine/StateMachine.hpp"

namespace Oathbound {

    void StateMachine::Push(StateRef state) { _pending.push_back({Op::Push, std::move(state)}); }
    void StateMachine::Replace(StateRef state) { _pending.push_back({Op::Replace, std::move(state)}); }
    void StateMachine::Pop() { _pending.push_back({Op::Pop, nullptr}); }

    void StateMachine::ApplyPending() {
        // OnEnter() may itself queue more changes: loop until the queue is really empty.
        while (!_pending.empty()) {
            std::vector<Command> commands = std::move(_pending);
            _pending.clear();

            for (Command& cmd : commands) {
                switch (cmd.op) {
                    case Op::Push:
                        if (!_stack.empty()) _stack.back()->OnPause();
                        _stack.push_back(std::move(cmd.state));
                        _stack.back()->OnEnter();
                        break;

                    case Op::Replace:
                        if (!_stack.empty()) {
                            _stack.back()->OnExit();
                            _stack.pop_back();
                        }
                        _stack.push_back(std::move(cmd.state));
                        _stack.back()->OnEnter();
                        break;

                    case Op::Pop:
                        if (!_stack.empty()) {
                            _stack.back()->OnExit();
                            _stack.pop_back();
                        }
                        if (!_stack.empty()) _stack.back()->OnResume();
                        break;
                }
            }
        }
    }

    void StateMachine::DrawAll() {
        if (_stack.empty()) return;

        std::size_t first = _stack.size() - 1;
        while (first > 0 && _stack[first]->IsOverlay()) --first;
        for (std::size_t i = first; i < _stack.size(); ++i) _stack[i]->Draw();
    }
}
