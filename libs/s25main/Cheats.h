// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "gameTypes/MapCoordinates.h"
#include <memory>

class CheatKeyTracker;
class GamePlayer;
class GameWorldBase;
struct KeyEvent;

class Cheats final
{
public:
    Cheats(GameWorldBase&);
    ~Cheats();

    void TrackKeyEvent(const KeyEvent&);

    void ToggleCheatMode();
    bool IsCheatModeOn() const noexcept { return isCheatModeOn_; }

    // Classic cheats
    void SetGameSpeed(uint8_t speedIndex);

    void ToggleAllVisible();
    bool IsAllVisible() const noexcept { return isAllVisible_; }

    bool CanPlaceCheatBuilding(const MapPoint&) const;
    void PlaceCheatBuilding(const MapPoint&, const GamePlayer&);

    // RTTR cheats
    void ToggleHumanAIPlayer();
    void RevealResources();

private:
    std::unique_ptr<CheatKeyTracker> keyTracker_;
    GameWorldBase& world_;
    bool isCheatModeOn_ = false;
    bool isAllVisible_ = false;
};
