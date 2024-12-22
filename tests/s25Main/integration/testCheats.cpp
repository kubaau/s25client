// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"
#include "GamePlayer.h"
#include "buildings/nobHQ.h"
#include "desktops/dskGameInterface.h"
#include "network/GameClient.h"
#include "worldFixtures/CreateEmptyWorld.h"
#include "worldFixtures/WorldFixture.h"
#include "gameData/MilitaryConsts.h"
#include <turtle/mock.hpp>

BOOST_AUTO_TEST_SUITE(CheatsTests)

namespace {
constexpr auto numPlayers = 2;
constexpr auto worldWidth = 64;
constexpr auto worldHeight = 64;
struct CheatsFixture : WorldFixture<CreateEmptyWorld, numPlayers, worldWidth, worldHeight>
{
    CheatsFixture()
        : cheats{gameDesktop.GI_GetCheats()}, viewer{gameDesktop.GetView().GetViewer()}, p1HQPos{p1.GetHQPos()},
          p2HQPos{p2.GetHQPos()}
    {}

    dskGameInterface gameDesktop{game, nullptr, 0, false};
    Cheats& cheats;
    const GameWorldViewer& viewer;

    GamePlayer& getPlayer(unsigned id) { return world.GetPlayer(id); }
    GamePlayer& p1 = getPlayer(0);
    GamePlayer& p2 = getPlayer(1);

    const MapPoint p1HQPos;
    const MapPoint p2HQPos;

    MapPoint unownedPt = {static_cast<MapCoord>(p1HQPos.x + HQ_RADIUS + 2), p1HQPos.y};

    auto countHQs(const GamePlayer& player)
    {
        return player.GetBuildingRegister().GetBuildingNums().buildings[BuildingType::Headquarters];
    }
    auto getHQs(const GamePlayer& player)
    {
        std::vector<nobHQ*> ret;
        for(auto bld : player.GetBuildingRegister().GetStorehouses())
            if(bld->GetBuildingType() == BuildingType::Headquarters)
                ret.push_back(static_cast<nobHQ*>(bld));
        return ret;
    }
};
} // namespace

BOOST_FIXTURE_TEST_CASE(CanToggleCheatModeOnAndOffRepeatedly, CheatsFixture)
{
    BOOST_TEST_REQUIRE(cheats.isCheatModeOn() == false); // initially off
    cheats.toggleCheatMode();
    BOOST_TEST_REQUIRE(cheats.isCheatModeOn() == true);
    cheats.toggleCheatMode();
    BOOST_TEST_REQUIRE(cheats.isCheatModeOn() == false);
    cheats.toggleCheatMode();
    BOOST_TEST_REQUIRE(cheats.isCheatModeOn() == true);
    cheats.toggleCheatMode();
    BOOST_TEST_REQUIRE(cheats.isCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(TurningCheatModeOffDisablesAllCheats, CheatsFixture)
{
    cheats.toggleCheatMode();
    cheats.toggleAllVisible();
    cheats.toggleAllBuildingsEnabled();
    cheats.toggleShowEnemyProductivityOverlay();
    cheats.setGameSpeed(Cheats::GameSpeed::SPEED_6);
    BOOST_TEST_REQUIRE(cheats.isAllVisible() == true);
    BOOST_TEST_REQUIRE(cheats.areAllBuildingsEnabled() == true);
    BOOST_TEST_REQUIRE(cheats.shouldShowEnemyProductivityOverlay() == true);
    BOOST_TEST_REQUIRE(GAMECLIENT.framesToSkipOnEachDraw_ > 0);

    cheats.toggleCheatMode();
    BOOST_TEST_REQUIRE(cheats.isAllVisible() == false);
    BOOST_TEST_REQUIRE(cheats.areAllBuildingsEnabled() == false);
    BOOST_TEST_REQUIRE(cheats.shouldShowEnemyProductivityOverlay() == false);
    BOOST_TEST_REQUIRE(GAMECLIENT.framesToSkipOnEachDraw_ == 0);
    // testing toggleHumanAIPlayer would require GameClient::state==Loaded, which is guaranteed in code (because Cheats
    // only exist after the game is loaded) but is not the case in tests - skipping
}

namespace {
MOCK_BASE_CLASS(MockGameInterface, GameInterface)
{
    // LCOV_EXCL_START
    MOCK_METHOD(GI_PlayerDefeated, 1)
    MOCK_METHOD(GI_UpdateMinimap, 1)
    MOCK_METHOD(GI_FlagDestroyed, 1)
    MOCK_METHOD(GI_TreatyOfAllianceChanged, 1)
    MOCK_METHOD(GI_UpdateMapVisibility, 0)
    MOCK_METHOD(GI_Winner, 1)
    MOCK_METHOD(GI_TeamWinner, 1)
    MOCK_METHOD(GI_StartRoadBuilding, 2)
    MOCK_METHOD(GI_CancelRoadBuilding, 0)
    MOCK_METHOD(GI_BuildRoad, 0)
    // clang-format off
    MOCK_METHOD(GI_GetCheats, 0, Cheats&(void))
    // clang-format on
    // LCOV_EXCL_STOP
};
} // namespace

BOOST_FIXTURE_TEST_CASE(CanToggleAllVisible_IfCheatModeIsOn, CheatsFixture)
{
    MockGameInterface mgi;
    MOCK_EXPECT(mgi.GI_GetCheats).returns(std::ref(gameDesktop.GI_GetCheats()));
    MOCK_EXPECT(mgi.GI_UpdateMapVisibility).exactly(3); // how many toggles should actually occur
    world.SetGameInterface(&mgi);

    MapPoint farawayPos = p1HQPos;
    farawayPos.x += 20;

    // initially farawayPos is not visible
    BOOST_TEST_REQUIRE((viewer.GetVisibility(farawayPos) == Visibility::Visible) == false);

    cheats.toggleAllVisible();
    // still not visible - cheat mode is not on
    BOOST_TEST_REQUIRE((viewer.GetVisibility(farawayPos) == Visibility::Visible) == false);

    cheats.toggleCheatMode();
    cheats.toggleAllVisible();
    // now visible - cheat mode is on
    BOOST_TEST_REQUIRE((viewer.GetVisibility(farawayPos) == Visibility::Visible) == true);

    cheats.toggleAllVisible();
    BOOST_TEST_REQUIRE((viewer.GetVisibility(farawayPos) == Visibility::Visible) == false);
    cheats.toggleAllVisible();
    BOOST_TEST_REQUIRE((viewer.GetVisibility(farawayPos) == Visibility::Visible) == true);
}

BOOST_FIXTURE_TEST_CASE(CanToggleAllBuildingsEnabled_AndShowEnemyProductivityOverlay_IfCheatModeIsOn, CheatsFixture)
{
    BOOST_TEST_REQUIRE(cheats.areAllBuildingsEnabled() == false);
    BOOST_TEST_REQUIRE(cheats.shouldShowEnemyProductivityOverlay() == false);

    cheats.toggleAllBuildingsEnabled();
    cheats.toggleShowEnemyProductivityOverlay();
    BOOST_TEST_REQUIRE(cheats.areAllBuildingsEnabled() == false);
    BOOST_TEST_REQUIRE(cheats.shouldShowEnemyProductivityOverlay() == false);

    cheats.toggleCheatMode();
    cheats.toggleAllBuildingsEnabled();
    cheats.toggleShowEnemyProductivityOverlay();
    BOOST_TEST_REQUIRE(cheats.areAllBuildingsEnabled() == true);
    BOOST_TEST_REQUIRE(cheats.shouldShowEnemyProductivityOverlay() == true);

    cheats.toggleAllBuildingsEnabled();
    cheats.toggleShowEnemyProductivityOverlay();
    BOOST_TEST_REQUIRE(cheats.areAllBuildingsEnabled() == false);
    BOOST_TEST_REQUIRE(cheats.shouldShowEnemyProductivityOverlay() == false);

    cheats.toggleAllBuildingsEnabled();
    cheats.toggleShowEnemyProductivityOverlay();
    BOOST_TEST_REQUIRE(cheats.areAllBuildingsEnabled() == true);
    BOOST_TEST_REQUIRE(cheats.shouldShowEnemyProductivityOverlay() == true);
}

BOOST_FIXTURE_TEST_CASE(CanBuildHQOutsideOwnedTerritory_AndDistanceFromBorderShouldBeAtLeastTwo, CheatsFixture)
{
    // cheats not enabled - cannot place
    BOOST_TEST_REQUIRE(cheats.canBuildHQ(unownedPt) == false);

    cheats.toggleCheatMode();

    // not in P1 territory
    MapPoint p1territory = p1HQPos;
    p1territory.x += 3;
    p1territory.y += 3;
    BOOST_TEST_REQUIRE(cheats.canBuildHQ(p1territory) == false);

    // not in P2 territory
    MapPoint p2territory = p2HQPos;
    p2territory.x += 3;
    p2territory.y += 3;
    BOOST_TEST_REQUIRE(cheats.canBuildHQ(p2territory) == false);

    // not at border
    MapPoint border = p1HQPos;
    border.x += HQ_RADIUS;
    BOOST_TEST_REQUIRE(cheats.canBuildHQ(border) == false);

    // not one node further from the border
    MapPoint nodeBeyondBorder = border;
    ++border.x;
    BOOST_TEST_REQUIRE(cheats.canBuildHQ(nodeBeyondBorder) == false);

    // further away - OK
    BOOST_TEST_REQUIRE(cheats.canBuildHQ(unownedPt) == true);
}

BOOST_FIXTURE_TEST_CASE(PlacesHQWithTheSameNationAndTentFlag, CheatsFixture)
{
    p1.SetHQIsTent(true);

    // initially 1 HQ
    BOOST_TEST_REQUIRE(countHQs(p1) == 1);

    cheats.buildHQ(unownedPt);
    BOOST_TEST_REQUIRE(countHQs(p1) == 1); // still 1 HQ - cheats are off

    // enable cheats and place HQ
    cheats.toggleCheatMode();
    cheats.buildHQ(unownedPt);
    BOOST_TEST_REQUIRE(countHQs(p1) == 2);
    for(auto bld : getHQs(p1))
    {
        BOOST_TEST_REQUIRE((bld->GetNation() == p1.nation) == true);
        BOOST_TEST_REQUIRE(static_cast<nobHQ*>(bld)->IsTent() == true);
    }
}

BOOST_FIXTURE_TEST_CASE(CanChangeGameSpeed_IfCheatModeIsOn, CheatsFixture)
{
    const auto initialGFLengthReq = GAMECLIENT.GetGFLengthReq();
    cheats.setGameSpeed(Cheats::GameSpeed::SPEED_6);
    // no change - cheats are off
    BOOST_TEST_REQUIRE(GAMECLIENT.GetGFLengthReq().count() == initialGFLengthReq.count());
    BOOST_TEST_REQUIRE(GAMECLIENT.framesToSkipOnEachDraw_ == 0);

    cheats.toggleCheatMode();
    cheats.setGameSpeed(Cheats::GameSpeed::SPEED_6);
    BOOST_TEST_REQUIRE(GAMECLIENT.framesToSkipOnEachDraw_ > 0);
    cheats.setGameSpeed(Cheats::GameSpeed::SPEED_1);
    BOOST_TEST_REQUIRE(GAMECLIENT.framesToSkipOnEachDraw_ == 0);
}

BOOST_AUTO_TEST_SUITE_END()
