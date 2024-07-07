// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>

class Cheats;
struct KeyEvent;

class CheatKeyTracker final
{
public:
    CheatKeyTracker(Cheats&);

    void TrackKeyEvent(const KeyEvent&);

private:
    bool TrackSpecialKeyEvent(const KeyEvent&);
    bool TrackSpeedKeyEvent(const KeyEvent&);
    bool TrackCharKeyEvent(const KeyEvent&);

    Cheats& cheats_;
    uint8_t cheatStrIndex_ = 0;
};
