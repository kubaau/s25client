// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"
#include "GameInterface.h"
#include "driver/KeyEvent.h"
#include "worldFixtures/CreateEmptyWorld.h"
#include "worldFixtures/WorldFixture.h"
#include <turtle/mock.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(CheatsTests)

namespace {
using EmptyWorldFixture1P = WorldFixture<CreateEmptyWorld, 1>;

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
    cheats.TrackKeyEvent(MakeKeyEvent(KeyType::Down));
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

    MOCK_EXPECT(gi.GI_UpdateMapVisibility).never();
    cheats.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == false);
}

BOOST_FIXTURE_TEST_CASE(CanToggleAllVisible_WhenCheatModeIsOn, EmptyWorldFixture1P)
{
    MockGameInterface gi;

    TrackString(cheats, "winter");
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    cheats.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == true);
}

BOOST_FIXTURE_TEST_CASE(CanToggleAllVisibleOnAndOff, EmptyWorldFixture1P)
{
    MockGameInterface gi;

    TrackString(cheats, "winter");
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    cheats.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == true);

    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    cheats.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == false);
}

BOOST_FIXTURE_TEST_CASE(CannotToggleAllVisibleOff_AfterTogglingItOnAndDisablingCheatMode, EmptyWorldFixture1P)
{
    MockGameInterface gi;

    TrackString(cheats, "winter");
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    cheats.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == true);

    TrackString(cheats, "winter");
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).never();
    cheats.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(cheats.IsAllVisible() == true);
}

BOOST_AUTO_TEST_SUITE_END()
