#pragma once

#include <SFML/Graphics.hpp>

namespace Oathbound {

    struct GameContext;

    // One screen of the game (splash, menu, gameplay, inventory overlay...).
    //
    // Differences with the old interface:
    //  - HandleEvent(event) is called once per event. The old HandleInput() polled the window
    //    itself and used sf::Mouse::isButtonPressed, so a held click re-triggered every frame.
    //  - Draw() has no unused parameter; the fixed-step loop lives in Application.
    //  - The fade/clock members that only the splash used no longer sit in the base class.
    class State {
    public:
        explicit State(GameContext& context) : _ctx(context) {}
        virtual ~State() = default;

        State(const State&) = delete;
        State& operator=(const State&) = delete;

        virtual void OnEnter() {}                         // once, when the state becomes active for the first time
        virtual void OnExit() {}                          // once, just before it is destroyed
        virtual void OnPause() {}                         // another state was pushed on top
        virtual void OnResume() {}                        // the state on top was popped

        virtual void HandleEvent(const sf::Event& event) = 0;
        virtual void Update(float dt) = 0;                // fixed time step
        virtual void Draw() = 0;

        // If true, the state below is drawn first (inventory over gameplay, pause menus...).
        virtual bool IsOverlay() const { return false; }

    protected:
        GameContext& _ctx;
    };
}
