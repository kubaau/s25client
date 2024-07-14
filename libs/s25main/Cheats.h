// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "gameTypes/MapCoordinates.h"

class GameInterface;
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
    void ToggleAllVisible(GameInterface&);
    bool IsAllVisible() const noexcept { return isAllVisible; }

    bool CanPlaceCheatBuilding(const MapPoint&) const;
    void PlaceCheatBuilding(const MapPoint&, const GamePlayer&);

    // RTTR cheats
    void ToggleHumanAIPlayer();
    void RevealResources();

private:
    GameWorldBase& world;
    unsigned char cheatStrIndex = 0;
    bool isCheatModeOn = false;
    bool isAllVisible = false;
};
