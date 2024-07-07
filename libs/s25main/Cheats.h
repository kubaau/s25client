// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "gameTypes/MapCoordinates.h"
#include "s25util/Singleton.h"

class GameInterface;
class GameWorldView;
class GameWorldViewer;
struct KeyEvent;

class Cheats : public Singleton<Cheats>
{
public:
    Cheats();

    void Reset();

    void TrackKeyEvent(const KeyEvent&, const GameWorldViewer&);
    bool IsCheatModeOn() const noexcept { return isCheatModeOn; }

    // Classic cheats
    void ToggleAllVisible(GameInterface&);
    bool IsAllVisible() const noexcept { return isAllVisible; }

    bool CanPlaceCheatBuilding(const GameWorldViewer&, const MapPoint&) const;
    void PlaceCheatBuilding(GameWorldView&, const MapPoint&);

    // RTTR cheats
    void ToggleHumanAIPlayer();

private:
    bool IsCheatModeOnOrDebug() const noexcept;

    unsigned char cheatStrIndex;
    bool isCheatModeOn;
    bool isAllVisible;
};

#define CHEATS Cheats::inst()
