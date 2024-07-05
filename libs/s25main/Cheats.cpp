// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"

#include "GameInterface.h"
#include "GamePlayer.h"
#include "buildings/nobHQ.h"
#include "driver/KeyEvent.h"
#include "factories/BuildingFactory.h"
#include "network/GameClient.h"
#include "world/GameWorldBase.h"
#include "world/GameWorldView.h"
#include "world/GameWorldViewer.h"
#include "gameTypes/MapNode.h"

Cheats::Cheats()
{
    Reset();
}

void Cheats::Reset()
{
    cheatStrIndex = 0;
    isCheatModeOn = false;
    isAllVisible = false;
}

void Cheats::TrackKeyEvent(const KeyEvent& ke, const GameWorldViewer& viewer)
{
    // In the original game, you would have to press the keys for the cheat string in the exact order without any
    // interruptions. For example, clicking CTRL (or any other button) after "w" would mean you have to retype "winter"
    // from scratch. RTTR doesn't track some keystrokes this way, so you can type "w", then CTRL, then "inter" and
    // cheats will be enabled, but typing "waaainter" or "w" F3 "inter" will not work.
    static const std::string cheatStr = "winter";
    RTTR_Assert(!cheatStr.empty());

    if(ke.kt != KeyType::Char)
    {
        cheatStrIndex = 0;
        return;
    }

    const char c = ke.c;
    if(c != cheatStr[cheatStrIndex])
    {
        cheatStrIndex = c == cheatStr.front() ? 1 : 0;
        return;
    }

    if(++cheatStrIndex == cheatStr.length())
    {
        isCheatModeOn = !isCheatModeOn;
        cheatStrIndex = 0;
    }
}

void Cheats::ToggleAllVisible(GameInterface& gi)
{
    // This is actually the behavior of the original game.
    // If you enabled cheats, revealed the map and disabled cheats you would be unable to unreveal the map.
    if(!IsCheatModeOn())
        return;

    isAllVisible = !isAllVisible;
    // The minimap in the original game is not updated immediately, but here this would cause complications.
    gi.GI_UpdateMapVisibility();
}

bool Cheats::CanPlaceCheatBuilding(const GameWorldViewer& viewer, const MapPoint& mp) const
{
    // The new HQ will have default resources.
    // In the original game, new HQs created in the Roman campaign had no resources.

    if(!IsCheatModeOn())
        return false;

    if(viewer.GetNode(mp).owner)
        return false;

    // It seems that in the original game you can only build headquarters in unoccupied territory at least 2 tiles
    // away from any border markers.
    for(const MapPoint& nb : viewer.GetNeighbours(mp))
        if(viewer.GetNode(nb).owner)
            return false;

    return viewer.GetNode(mp).bq >= BuildingQuality::Hut;
}

void Cheats::PlaceCheatBuilding(GameWorldView& gwv, const MapPoint& mp)
{
    if(!CanPlaceCheatBuilding(gwv.GetViewer(), mp))
        return;

    GameWorldBase& world = const_cast<GameWorldBase&>(gwv.GetWorld());
    constexpr auto checkExists = false;
    world.DestroyNO(mp, checkExists); // if CanPlaceCheatBuilding is true then this must be safe to destroy
    const GamePlayer& player = gwv.GetViewer().GetPlayer();
    const nobHQ* hq = player.GetHQ();
    BuildingFactory::CreateBuilding(world, BuildingType::Headquarters, mp, player.GetPlayerId(), player.nation,
                                    hq ? hq->IsTent() : false);
}

void Cheats::ToggleHumanAIPlayer()
{
    if(!IsCheatModeOn())
        return;

    if(GAMECLIENT.GetState() != ClientState::Game)
        return;

    if(GAMECLIENT.IsReplayModeOn())
        return;

    GAMECLIENT.ToggleHumanAIPlayer({AI::Type::Default, AI::Level::Hard});
}
