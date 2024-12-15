// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "gameTypes/MapCoordinates.h"

class GameWorldBase;

class Cheats
{
public:
    Cheats(GameWorldBase& world);

    bool areCheatsAllowed() const;

    void toggleCheatMode();
    bool isCheatModeOn() const { return isCheatModeOn_; }

    // Classic S2 cheats
    void toggleAllVisible();
    bool isAllVisible() const { return isAllVisible_; }

    bool canBuildHQ(const MapPoint& mp) const;
    void buildHQ(const MapPoint& mp);

    // RTTR cheats
    void toggleHumanAIPlayer() const;
    void armageddon() const;

private:
    bool isCheatModeOn_ = false;
    bool isAllVisible_ = false;
    GameWorldBase& world_;
};
