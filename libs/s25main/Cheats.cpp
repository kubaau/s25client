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
#include "nodeObjs/noSign.h"

Cheats::Cheats(GameWorldBase& world) : world_(world) {}

void Cheats::TrackKeyEvent(const KeyEvent& ke)
{
    TrackSpecialKeyEvent(ke) || TrackSpeedKeyEvent(ke) || TrackCharKeyEvent(ke);
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

bool Cheats::TrackSpecialKeyEvent(const KeyEvent& ke)
{
    if(ke.kt == KeyType::Char)
        return false;

    cheatStrIndex_ = 0;

    switch(ke.kt)
    {
        case KeyType::F7: ke.alt ? RevealResources() : ToggleAllVisible(); break;
        case KeyType::F10: ToggleHumanAIPlayer(); break;
        default: break;
    }
    return true;
}

bool Cheats::TrackSpeedKeyEvent(const KeyEvent& ke)
{
    const char c = ke.c;
    if(ke.alt && c >= '1' && c <= '6')
    {
        cheatStrIndex_ = 0;
        SetGameSpeed(c);
        return true;
    }
    return false;
}

bool Cheats::TrackCharKeyEvent(const KeyEvent& ke)
{
    // In the original game, you would have to press the keys for the cheat string in the exact order without any
    // interruptions. For example, clicking CTRL (or any other button) after "w" would mean you have to retype "winter"
    // from scratch. RTTR doesn't track some keystrokes this way, so you can type "w", then CTRL, then "inter" and
    // cheats will be enabled, but typing "waaainter" or "w" F3 "inter" will not work (see tests).
    static const std::string cheatStr = "winter";
    RTTR_Assert(!cheatStr.empty());

    const char c = ke.c;
    if(c != cheatStr[cheatStrIndex_])
    {
        cheatStrIndex_ = c == cheatStr.front() ? 1 : 0; // if 'w' then index = 1
        return true;
    }

    if(++cheatStrIndex_ == cheatStr.length())
    {
        isCheatModeOn_ = !isCheatModeOn_;
        cheatStrIndex_ = 0;
    }
    return true;
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

void Cheats::SetGameSpeed(char c)
{
    if(!IsCheatModeOn())
        return;

    // 50 -> 25 -> 12 -> 6 -> 3 -> 1
    GAMECLIENT.SetGFLengthReq(FramesInfo::milliseconds32_t{50 >> (c - '1')});
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
