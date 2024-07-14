// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "CheatKeyTracker.h"
#include "Cheats.h"
#include "driver/KeyEvent.h"

namespace {
auto MakeCircularBuffer(const std::string& str)
{
    return boost::circular_buffer<char>{cbegin(str), cend(str)};
}
const auto cheatStr = MakeCircularBuffer("winter");
} // namespace

CheatKeyTracker::CheatKeyTracker(Cheats& cheats) : cheats_(cheats), lastChars_(cheatStr.size()) {}

void CheatKeyTracker::TrackKeyEvent(const KeyEvent& ke)
{
    if(TrackSpecialKeyEvent(ke) || TrackSpeedKeyEvent(ke))
    {
        lastChars_.clear();
        return;
    }

    TrackCharKeyEvent(ke);
}

bool CheatKeyTracker::TrackSpecialKeyEvent(const KeyEvent& ke)
{
    if(ke.kt == KeyType::Char)
        return false;

    switch(ke.kt)
    {
        case KeyType::F7:
        {
            if(ke.ctrl && ke.shift && ke.alt)
                cheats_.DestroyAllAI();
            else if(ke.alt)
                cheats_.RevealResources();
            else
                cheats_.ToggleAllVisible();
        }
        break;
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

    lastChars_.push_back(ke.c);

    if(lastChars_ == cheatStr)
        cheats_.ToggleCheatMode();

    return true;
}
