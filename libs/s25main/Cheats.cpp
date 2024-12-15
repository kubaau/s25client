// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"
#include "GameInterface.h"
#include "GamePlayer.h"
#include "factories/BuildingFactory.h"
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
    // In S2, if you enabled cheats, revealed the map and disabled cheats, you would be unable to unreveal the map.
    // In RTTR, with cheats disabled, you can unreveal the map but cannot reveal it.
    if(isCheatModeOn() || isAllVisible())
    {
        isAllVisible_ = !isAllVisible_;

        // In S2, the minimap is not updated immediately.
        // In RTTR, the minimap would become messed up if it wasn't updated here.
        if(GameInterface* gi = world_.GetGameInterface())
            gi->GI_UpdateMapVisibility();
    }
}

bool Cheats::canBuildHQ(const MapPoint& mp) const
{
    // In S2, you can build headquarters in unoccupied territory at least 2 nodes away from any border markers and it
    // doesn't need more bq than a hut.
    return isCheatModeOn() && !world_.IsPointOrAnyNeighborOwned(mp) && world_.GetNode(mp).bq >= BuildingQuality::Hut;
}

void Cheats::buildHQ(const MapPoint& mp)
{
    if(canBuildHQ(mp))
    {
        // In S2, new HQs created in Roman-style campaigns had no resources.
        // In RTTR, all new HQs will have default resources.
        world_.DestroyNO(mp, /*checkExists*/ false); // if canBuildHQ is true then this is safe to destroy (due to bq)
        constexpr auto playerId = 0;
        BuildingFactory::CreateBuilding(world_, BuildingType::Headquarters, mp, playerId,
                                        world_.GetPlayer(playerId).nation);
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
