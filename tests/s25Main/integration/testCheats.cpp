// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Cheats.h"
#include "GameInterface.h"
#include "driver/KeyEvent.h"
#include "worldFixtures/CreateEmptyWorld.h"
#include "worldFixtures/WorldFixture.h"
#include "world/GameWorldViewer.h"
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

void TrackString(const GameWorldViewer& gwv, const std::string& str)
{
    for(char c : str)
        CHEATS.TrackKeyEvent(MakeKeyEvent(c), gwv);
}
} // namespace

BOOST_FIXTURE_TEST_CASE(CheatModeIsOffByDefault, EmptyWorldFixture1P)
{
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeCanBeTurnedOn, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == true);
}

BOOST_FIXTURE_TEST_CASE(CheatModeCanBeTurnedOff, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == true);
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeCanBeTurnedOnAndOffRepeatedly, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == true);
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == true);
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenInterruptedByAnotherKeyType, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "win");
    CHEATS.TrackKeyEvent(MakeKeyEvent(KeyType::F7), gwv);
    TrackString(gwv, "ter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenInterruptedByAnotherLetter, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "wainter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenOrderOfCharactersIsWrong, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "winetr");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsNotTurnedOn_WhenACharacterIsRepeated, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "winnter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsTurnedOn_WhenTheFirstCharacterIsRepeated, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "wwwinter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == true);
}

BOOST_FIXTURE_TEST_CASE(CheatModeIsTurnedOn_EvenWhenWrongInputsWereProvidedBefore, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    TrackString(gwv, "www");
    CHEATS.TrackKeyEvent(MakeKeyEvent(KeyType::Down), gwv);
    TrackString(gwv, "interwitter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == true);
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
    GameWorldViewer gwv{0, world};
    MockGameInterface gi;

    MOCK_EXPECT(gi.GI_UpdateMapVisibility).never();
    CHEATS.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == false);
}

BOOST_FIXTURE_TEST_CASE(CanToggleAllVisible_WhenCheatModeIsOn, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    MockGameInterface gi;

    TrackString(gwv, "winter");
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    CHEATS.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == true);
}

BOOST_FIXTURE_TEST_CASE(CanToggleAllVisibleOnAndOff, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    MockGameInterface gi;

    TrackString(gwv, "winter");
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    CHEATS.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == true);

    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    CHEATS.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == false);
}

BOOST_FIXTURE_TEST_CASE(CannotToggleAllVisibleOff_AfterTogglingItOnAndDisablingCheatMode, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    MockGameInterface gi;

    TrackString(gwv, "winter");
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    CHEATS.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == true);

    TrackString(gwv, "winter");
    MOCK_EXPECT(gi.GI_UpdateMapVisibility).never();
    CHEATS.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == true);
}

BOOST_FIXTURE_TEST_CASE(StateCanBeReset, EmptyWorldFixture1P)
{
    GameWorldViewer gwv{0, world};
    MockGameInterface gi;

    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == false);
    TrackString(gwv, "winter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == true);

    MOCK_EXPECT(gi.GI_UpdateMapVisibility).once();
    CHEATS.ToggleAllVisible(gi);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == true);

    TrackString(gwv, "win");
    CHEATS.Reset();
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
    TrackString(gwv, "ter");
    BOOST_TEST_REQUIRE(CHEATS.IsCheatModeOn() == false);
    BOOST_TEST_REQUIRE(CHEATS.IsAllVisible() == false);
}

BOOST_AUTO_TEST_SUITE_END()
