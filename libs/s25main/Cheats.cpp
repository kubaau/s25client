// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"
#include "GameInterface.h"
#include "GamePlayer.h"
#include "factories/BuildingFactory.h"
#include "network/GameClient.h"
#include "world/GameWorldBase.h"

using namespace std::literals;

Cheats::Cheats(GameWorldBase& world) : world_(world) {}

bool Cheats::areCheatsAllowed() const
{
    return world_.IsSinglePlayer();
}

void Cheats::toggleCheatMode()
{
    // In S2, if you enabled cheat mode, revealed the map and disabled cheat mode, the map would remain revealed and you
    // would be unable to unreveal the map.
    // In RTTR, disabling cheat mode turns all cheats off and they have to be turned on again manually.
    if(isCheatModeOn_)
        turnAllCheatsOff();

    isCheatModeOn_ = !isCheatModeOn_;
}

void Cheats::toggleAllVisible()
{
    if(isCheatModeOn())
    {
        isAllVisible_ = !isAllVisible_;

        // In S2, the minimap is not updated immediately.
        // In RTTR, the minimap would become messed up if it wasn't updated here.
        if(GameInterface* gi = world_.GetGameInterface())
            gi->GI_UpdateMapVisibility();
    }
}

void Cheats::toggleAllBuildingsEnabled()
{
    // In S2, if you enabled cheats you would automatically have all buildings enabled.
    // In RTTR, because this may have unintended consequences when playing campaigns, the user must explicitly enable
    // all buildings after enabling cheats.
    if(isCheatModeOn())
        areAllBuildingsEnabled_ = !areAllBuildingsEnabled_;
}

void Cheats::toggleShowEnemyProductivityOverlay()
{
    // In S2, if you enabled cheats you would automatically see the enemy productivity overlay - most importantly what
    // buildings the enemy intends to build.
    // In RTTR, the user must explicitly enable this feature after enabling cheats.
    if(isCheatModeOn())
        shouldShowEnemyProductivityOverlay_ = !shouldShowEnemyProductivityOverlay_;
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

        // If the following code wasn't there, one could build an HQ right at the enemy border and their military flags
        // wouldn't be updated. One could attempt a refactor of LookForEnemyBuildings, but it is simpler just to
        // recalculate the flags for everyone, since building an HQ can destroy a lot of stuff anyway and is rarely
        // done.
        world_.ForEachPlayer([](auto& player) { player.RecalcMilitaryFlags(); });
    }
}

void Cheats::setGameSpeed(GameSpeed speed)
{
    if(isCheatModeOn())
    {
        FramesInfo::milliseconds32_t gfLengthReq;
        unsigned framesToSkipOnEachDraw = 0;

        switch(speed)
        {
            case GameSpeed::SPEED_1:
            default: gfLengthReq = 50ms; break;
            case GameSpeed::SPEED_2: gfLengthReq = 30ms; break;
            case GameSpeed::SPEED_3: gfLengthReq = 20ms; break;
            case GameSpeed::SPEED_4: gfLengthReq = 10ms; break;
            case GameSpeed::SPEED_5: gfLengthReq = 1ms; break;
            case GameSpeed::SPEED_6:
                gfLengthReq = 1ms;
                framesToSkipOnEachDraw = 10;
                break;
        }

        GAMECLIENT.SetGFLengthReq(gfLengthReq, framesToSkipOnEachDraw);
    }
}

void Cheats::toggleHumanAIPlayer()
{
    if(isCheatModeOn() && !GAMECLIENT.IsReplayModeOn())
    {
        GAMECLIENT.ToggleHumanAIPlayer(AI::Info{AI::Type::Default, AI::Level::Easy});
        isHumanAIPlayer_ = !isHumanAIPlayer_;
    }
}

void Cheats::armageddon() const
{
    if(isCheatModeOn())
        GAMECLIENT.CheatArmageddon();
}

void Cheats::turnAllCheatsOff()
{
    if(isAllVisible_)
        toggleAllVisible();
    if(areAllBuildingsEnabled_)
        toggleAllBuildingsEnabled();
    if(shouldShowEnemyProductivityOverlay_)
        toggleShowEnemyProductivityOverlay();
    if(GAMECLIENT.framesToSkipOnEachDraw_)
        GAMECLIENT.SetGFLengthReq(1ms);
    if(isHumanAIPlayer_)
        toggleHumanAIPlayer();
}
