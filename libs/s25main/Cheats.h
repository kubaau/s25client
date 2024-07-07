// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "gameTypes/MapCoordinates.h"
#include "s25util/Singleton.h"

class GameInterface;
class GameWorldView;
class GameWorldViewer;

class Cheats : public Singleton<Cheats>
{
public:
    void ToggleCheatMode() { isCheatModeOn = !isCheatModeOn; }
    bool IsCheatModeOn() const noexcept { return isCheatModeOn; }
    bool IsCheatModeOnOrDebug() const noexcept;

    void ToggleAllVisible(GameInterface&);
    bool IsAllVisible() const noexcept { return isAllVisible; }

    bool CanPlaceCheatBuilding(const GameWorldViewer&, const MapPoint&) const;
    void PlaceCheatBuilding(GameWorldView&, const MapPoint&);

private:
    bool isCheatModeOn = false;
    bool isAllVisible = false;
};

#define CHEATS Cheats::inst()
