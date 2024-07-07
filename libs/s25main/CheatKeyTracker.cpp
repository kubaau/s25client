// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "CheatKeyTracker.h"
#include "Cheats.h"
#include "driver/KeyEvent.h"

CheatKeyTracker::CheatKeyTracker(Cheats& cheats) : cheats_(cheats) {}

void CheatKeyTracker::TrackKeyEvent(const KeyEvent& ke)
{
    TrackSpecialKeyEvent(ke) || TrackSpeedKeyEvent(ke) || TrackCharKeyEvent(ke);
}

bool CheatKeyTracker::TrackSpecialKeyEvent(const KeyEvent& ke)
{
    if(ke.kt == KeyType::Char)
        return false;

    cheatStrIndex_ = 0;

    switch(ke.kt)
    {
        case KeyType::F7: ke.alt ? cheats_.RevealResources() : cheats_.ToggleAllVisible(); break;
        case KeyType::F10: cheats_.ToggleHumanAIPlayer(); break;
        default: break;
    }
    return true;
}

bool CheatKeyTracker::TrackSpeedKeyEvent(const KeyEvent& ke)
{
    const char c = ke.c;
    if(ke.alt && c >= '1' && c <= '6')
    {
        cheatStrIndex_ = 0;
        cheats_.SetGameSpeed(c - '1');
        return true;
    }
    return false;
}

bool CheatKeyTracker::TrackCharKeyEvent(const KeyEvent& ke)
{
    // In the original game, you would have to press the keys for the cheat string in the exact order without any
    // interruptions. For example, clicking CTRL (or any other button) after "w" would mean you have to retype "winter"
    // from scratch. RTTR doesn't track some keystrokes this way, so you can type "w", then CTRL, then "inter" and
    // cheats will be enabled, but typing "waaainter" or "w" F3 "inter" will not work (see tests).
    constexpr auto cheatStr = "winter";
    static_assert(cheatStr[0], "cheatStr must not be empty");

    const char c = ke.c;
    if(c != cheatStr[cheatStrIndex_])
    {
        cheatStrIndex_ = c == cheatStr[0] ? 1 : 0; // if 'w' then index = 1
        return true;
    }

    if(!cheatStr[++cheatStrIndex_])
    {
        cheats_.ToggleCheatMode();
        cheatStrIndex_ = 0;
    }

    return true;
}
