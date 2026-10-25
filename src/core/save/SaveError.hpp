#pragma once

#include <stdexcept>

namespace Oathbound {

    // Anything that prevents a save from being read, understood or written.
    class SaveError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    // The file was written by a NEWER version of the game. Never "recovered" from an older
    // backup: that would silently roll the player's progress back.
    class SaveTooNewError : public SaveError {
    public:
        using SaveError::SaveError;
    };
}
