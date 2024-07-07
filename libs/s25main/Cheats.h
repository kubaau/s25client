// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "gameTypes/MapCoordinates.h"

class GamePlayer;
class GameWorldBase;
struct KeyEvent;

class Cheats final
{
public:
    Cheats(GameWorldBase&);

    void TrackKeyEvent(const KeyEvent&);
    bool IsCheatModeOn() const noexcept { return isCheatModeOn; }

    // Classic cheats
    bool IsAllVisible() const noexcept { return isAllVisible; }

    bool CanPlaceCheatBuilding(const MapPoint&) const;
    void PlaceCheatBuilding(const MapPoint&, const GamePlayer&);

private:
    bool TrackSpecialKeyEvent(const KeyEvent&);
    bool TrackSpeedKeyEvent(const KeyEvent&);
    bool TrackCharKeyEvent(const KeyEvent&);

    // Classic cheats
    void ToggleAllVisible();
    void SetGameSpeed(char);

    // RTTR cheats
    void RevealResources();
    void ToggleHumanAIPlayer();

    GameWorldBase& world;
    unsigned char cheatStrIndex = 0;
    bool isCheatModeOn = false;
    bool isAllVisible = false;
};
