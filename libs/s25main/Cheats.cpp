// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"
#include "GameInterface.h"
#include "network/GameClient.h"
#include "world/GameWorldBase.h"

Cheats::Cheats(GameWorldBase& world) : world_(world) {}

bool Cheats::areCheatsAllowed() const
{
    return world_.IsSinglePlayer();
}

void Cheats::toggleCheatMode()
{
    isCheatModeOn_ = !isCheatModeOn_;
}

void Cheats::toggleAllVisible()
{
    // In the original game if you enabled cheats, revealed the map and disabled cheats, you would be unable to unreveal
    // the map. In RTTR, with cheats disabled, you can unreveal the map but cannot reveal it.
    if(isCheatModeOn() || isAllVisible())
    {
        isAllVisible_ = !isAllVisible_;

        // The minimap in the original game is not updated immediately, but in RTTR this would mess up the minimap.
        if(GameInterface* gi = world_.GetGameInterface())
            gi->GI_UpdateMapVisibility();
    }
}

void Cheats::setGameSpeed(unsigned speedIndex)
{
    if(!isCheatModeOn())
        return;

    switch(speedIndex)
    {
        case 0: return GAMECLIENT.SetGFLengthReq(FramesInfo::milliseconds32_t{50});
        case 1: return GAMECLIENT.SetGFLengthReq(FramesInfo::milliseconds32_t{25});
        case 2: return GAMECLIENT.SetGFLengthReq(FramesInfo::milliseconds32_t{10});
        case 3: return GAMECLIENT.SetGFLengthReq(FramesInfo::milliseconds32_t{1}, false);
        case 4: return GAMECLIENT.SetGFLengthReq(FramesInfo::milliseconds32_t{1}, false, 10);
        case 5: return GAMECLIENT.SetGFLengthReq(FramesInfo::milliseconds32_t{1}, false, 500);
        default: RTTR_Assert(false); // should never happen
    }
}

void Cheats::toggleHumanAIPlayer() const
{
    if(isCheatModeOn() && !GAMECLIENT.IsReplayModeOn())
        GAMECLIENT.ToggleHumanAIPlayer(AI::Info{AI::Type::Default, AI::Level::Easy});
}

void Cheats::armageddon() const
{
    if(isCheatModeOn())
        GAMECLIENT.CheatArmageddon();
}
