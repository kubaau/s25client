// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"
#include "CheatKeyTracker.h"
#include "GameInterface.h"
#include "GamePlayer.h"
#include "buildings/nobHQ.h"
#include "factories/BuildingFactory.h"
#include "network/GameClient.h"
#include "world/GameWorldBase.h"
#include "nodeObjs/noSign.h"

Cheats::Cheats(GameWorldBase& world) : keyTracker_(std::make_unique<CheatKeyTracker>(*this)), world_(world) {}

Cheats::~Cheats() = default;

void Cheats::TrackKeyEvent(const KeyEvent& ke)
{
    keyTracker_->TrackKeyEvent(ke);
}

void Cheats::ToggleCheatMode()
{
    isCheatModeOn_ = !isCheatModeOn_;
}

void Cheats::SetGameSpeed(uint8_t speedIndex)
{
    if(!IsCheatModeOn())
        return;

    constexpr auto gfLengthInMs = 50;
    GAMECLIENT.SetGFLengthReq(FramesInfo::milliseconds32_t{gfLengthInMs >> speedIndex});
    // 50 -> 25 -> 12 -> 6 -> 3 -> 1
}

void Cheats::ToggleAllVisible()
{
    // This is actually the behavior of the original game.
    // If you enabled cheats, revealed the map and disabled cheats you would be unable to unreveal the map.
    if(!IsCheatModeOn())
        return;

    isAllVisible_ = !isAllVisible_;
    // The minimap in the original game is not updated immediately, but here this would cause complications.
    if(GameInterface* gi = world_.GetGameInterface())
        gi->GI_UpdateMapVisibility();
}

bool Cheats::CanPlaceCheatBuilding(const MapPoint& mp) const
{
    if(!IsCheatModeOn())
        return false;

    // It seems that in the original game you can only build headquarters in unoccupied territory at least 2 nodes
    // away from any border markers and that it doesn't need more bq than a hut.
    const MapNode& node = world_.GetNode(mp);
    return !node.owner && !world_.IsAnyNeighborOwned(mp) && node.bq >= BuildingQuality::Hut;
}

void Cheats::PlaceCheatBuilding(const MapPoint& mp, const GamePlayer& player)
{
    if(!CanPlaceCheatBuilding(mp))
        return;

    // The new HQ will have default resources.
    // In the original game, new HQs created in the Roman campaign had no resources.
    constexpr auto checkExists = false;
    world_.DestroyNO(mp, checkExists); // if CanPlaceCheatBuilding is true then this must be safe to destroy
    const nobHQ* hq = player.GetHQ();
    BuildingFactory::CreateBuilding(world_, BuildingType::Headquarters, mp, player.GetPlayerId(), player.nation,
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

void Cheats::RevealResources()
{
    if(!IsCheatModeOn())
        return;

    const auto width = world_.GetWidth();
    const auto height = world_.GetHeight();

    for(MapCoord y = 0; y < height; ++y)
        for(MapCoord x = 0; x < width; ++x)
        {
            const MapPoint mp = {x, y};

            const NodalObjectType noType = world_.GetNO(mp)->GetType();
            if(noType != NodalObjectType::Nothing && noType != NodalObjectType::Environment)
                continue; // don't want to destroy this object to place a sign

            Resource res = world_.GetNode(mp).resources;

            switch(res.getType())
            {
                // show these
                case ResourceType::Iron:
                case ResourceType::Gold:
                case ResourceType::Coal:
                case ResourceType::Granite: break;

                // water is typically almost everywhere, don't reveal, continue to next node
                case ResourceType::Water: continue;

                // show fish as water, why the hell not
                case ResourceType::Fish: res.setType(ResourceType::Water); break;

                // no resource to reveal, continue to next node
                default: continue;
            }

            constexpr auto checkExists = false;
            world_.DestroyNO(mp, checkExists); // safe to destroy because we checked this earlier
            if(res.getAmount())
                world_.SetNO(mp, new noSign(mp, res));
        }
}
