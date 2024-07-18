// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"
#include "GameInterface.h"
#include "GamePlayer.h"
#include "driver/KeyEvent.h"
#include "worldFixtures/CreateEmptyWorld.h"
#include "worldFixtures/WorldFixture.h"
#include <turtle/mock.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(CheatsTests)

namespace {
using EmptyWorldFixture1P = WorldFixture<CreateEmptyWorld, 1>;
using EmptyWorldFixture2P = WorldFixture<CreateEmptyWorld, 2>;
using EmptyWorldFixture3P = WorldFixture<CreateEmptyWorld, 3>;

KeyEvent MakeKeyEvent(unsigned c)
{
    return {KeyType::Char, c, 0, 0, 0};
}
KeyEvent MakeKeyEvent(KeyType kt)
{
    return {kt, 0, 0, 0, 0};
}

void TrackString(Cheats& cheats, const std::string& str)
{
    for(char c : str)
        cheats.TrackKeyEvent(MakeKeyEvent(c));
}
} // namespace

BOOST_FIXTURE_TEST_CASE(CheatModeIsOffByDefault, EmptyWorldFixture1P)
{
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeCanBeTurnedOn, EmptyWorldFixture1P)
{
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == true);
}

BOOST_FIXTURE_TEST_CASE(CheatModeCannotBeTurnedOn_InMultiplayer, EmptyWorldFixture2P)
{
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeCanBeTurnedOff, EmptyWorldFixture1P)
{
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == true);
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeCanBeTurnedOnAndOffRepeatedly, EmptyWorldFixture1P)
{
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == true);
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == true);
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenIncomplete, EmptyWorldFixture1P)
{
    TrackString(cheats, "winte");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenInterruptedByAnotherKeyType, EmptyWorldFixture1P)
{
    TrackString(cheats, "win");
    cheats.TrackKeyEvent(MakeKeyEvent(KeyType::F7));
    TrackString(cheats, "ter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenInterruptedByAnotherLetter, EmptyWorldFixture1P)
{
    TrackString(cheats, "wainter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenOrderOfCharactersIsWrong, EmptyWorldFixture1P)
{
    TrackString(cheats, "winetr");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenOrderOfCharactersIsWrong_Wraparound, EmptyWorldFixture1P)
{
    TrackString(cheats, "rwinte");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenACharacterIsRepeated, EmptyWorldFixture1P)
{
    TrackString(cheats, "winnter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsTurnedOn_WhenTheFirstCharacterIsRepeated, EmptyWorldFixture1P)
{
    TrackString(cheats, "wwwinter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == true);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsTurnedOn_EvenWhenWrongInputsWereProvidedBefore, EmptyWorldFixture1P)
{
    TrackString(cheats, "www");
    auto ke = MakeKeyEvent('1');
    ke.alt = true;
    cheats.TrackKeyEvent(ke);
    TrackString(cheats, "interwitter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == false);
    TrackString(cheats, "winter");
    BOOST_TEST_REQUIRE(cheats.IsCheatModeOn() == true);
}

MOCK_BASE_CLASS(MockGameInterface, GameInterface)
{
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
};

BOOST_FIXTURE_TEST_CASE(CannotToggleAllVisible_WhenCheatModeIsNotOn, EmptyWorldFixture1P)
{
    MockGameInterface gi;
    world.SetGameInterface(&gi);

    MOCK_EXPECT(gi.GI_UpdateMapVisibility).never();
    cheats.ToggleAllVisible();
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == false);
}

BOOST_FIXTURE_TEST_CASE(CanToggleAllVisible_WhenCheatModeIsOn, EmptyWorldFixture1P)
{
    MockGameInterface gi;
    world.SetGameInterface(&gi);

    cheats.ToggleCheatMode();
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    cheats.ToggleAllVisible();
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == true);
}

BOOST_FIXTURE_TEST_CASE(CanToggleAllVisibleOnAndOff, EmptyWorldFixture1P)
{
    MockGameInterface gi;
    world.SetGameInterface(&gi);

    cheats.ToggleCheatMode();
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    cheats.ToggleAllVisible();
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == true);

    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    cheats.ToggleAllVisible();
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == false);
}

BOOST_FIXTURE_TEST_CASE(CannotToggleAllVisibleOff_AfterTogglingItOnAndDisablingCheatMode, EmptyWorldFixture1P)
{
    MockGameInterface gi;
    world.SetGameInterface(&gi);

    cheats.ToggleCheatMode();
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    cheats.ToggleAllVisible();
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == true);

    cheats.ToggleCheatMode();
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).never();
    cheats.ToggleAllVisible();
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == true);
}

BOOST_FIXTURE_TEST_CASE(CanToggleResourcesToRevealSuccessively, EmptyWorldFixture1P)
{
    using RRM = Cheats::ResourceRevealMode;

    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Nothing);
    cheats.ToggleCheatMode();
    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Nothing);
    cheats.ToggleResourceRevealMode();
    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Ores);
    cheats.ToggleResourceRevealMode();
    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Fish);
    cheats.ToggleResourceRevealMode();
    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Water);
    cheats.ToggleResourceRevealMode();
    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Nothing);
    cheats.ToggleResourceRevealMode();
    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Ores);
    cheats.ToggleCheatMode();
    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Nothing);
    cheats.ToggleResourceRevealMode();
    cheats.ToggleCheatMode();
    BOOST_CHECK(cheats.GetResourceRevealMode() == RRM::Fish);
}

namespace {
auto countAllBuildings(const GamePlayer& player)
{
    unsigned ret = 0;
    for(auto b : player.GetBuildingRegister().GetBuildingNums().buildings)
        ret += b;
    return ret;
}
} // namespace

BOOST_FIXTURE_TEST_CASE(DestroyBuildingsOfGivenPlayer, EmptyWorldFixture2P)
{
    cheats.ToggleCheatMode();
    const GamePlayer& p1 = world.GetPlayer(0);
    const GamePlayer& p2 = world.GetPlayer(1);
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 1);
    BOOST_TEST_REQUIRE(countAllBuildings(p2) == 1);
    cheats.DestroyBuildings({0});
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 0);
    BOOST_TEST_REQUIRE(countAllBuildings(p2) == 1);
}

BOOST_FIXTURE_TEST_CASE(DestroyBuildingsOfGivenPlayers, EmptyWorldFixture2P)
{
    cheats.ToggleCheatMode();
    const GamePlayer& p1 = world.GetPlayer(0);
    const GamePlayer& p2 = world.GetPlayer(1);
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 1);
    BOOST_TEST_REQUIRE(countAllBuildings(p2) == 1);
    cheats.DestroyBuildings({0, 1});
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 0);
    BOOST_TEST_REQUIRE(countAllBuildings(p2) == 0);
}

BOOST_FIXTURE_TEST_CASE(DestroyBuildingsOfAIPlayers, EmptyWorldFixture3P)
{
    cheats.ToggleCheatMode();
    const GamePlayer& p1 = world.GetPlayer(0);
    GamePlayer& p2 = world.GetPlayer(1);
    p2.ps = PlayerState::AI;
    GamePlayer& p3 = world.GetPlayer(2);
    p3.ps = PlayerState::AI;
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 1);
    BOOST_TEST_REQUIRE(countAllBuildings(p2) == 1);
    BOOST_TEST_REQUIRE(countAllBuildings(p3) == 1);
    cheats.DestroyAllAIBuildings();
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 1);
    BOOST_TEST_REQUIRE(countAllBuildings(p2) == 0);
    BOOST_TEST_REQUIRE(countAllBuildings(p3) == 0);
}

BOOST_FIXTURE_TEST_CASE(CannotDestroyBuildings_IfCheatModeIsNotOn, EmptyWorldFixture1P)
{
    GamePlayer& p1 = world.GetPlayer(0);
    p1.ps = PlayerState::AI;
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 1);
    cheats.DestroyBuildings({0});
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 1);
    cheats.DestroyAllAIBuildings();
    BOOST_TEST_REQUIRE(countAllBuildings(p1) == 1);
}

BOOST_AUTO_TEST_SUITE_END()
