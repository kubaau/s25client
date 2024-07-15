// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "gameTypes/MapCoordinates.h"
#include <memory>

class CheatKeyTracker;
class GamePlayer;
class GameWorldBase;
struct KeyEvent;

class Cheats final
{
public:
    Cheats(GameWorldBase&);
    ~Cheats(); // default - needed for unique_ptr

    /** In single player games, tracks keyboard events related to cheats.
     * Delegates this responsibility to CheatKeyTracker, which triggers the actual cheats.
     * In multiplayer games, does nothing. No cheats can be triggered in multiplayer.
     *
     * @param ke - The keyboard event encountered.
     */
    void TrackKeyEvent(const KeyEvent& ke);

    /** Toggles cheat mode on and off.
     * Cheat mode needs to be on for any cheats to trigger.
     */
    void ToggleCheatMode();
    /** Used by clients to check if cheat mode is on (e.g. to draw a special sprite or enable or all buildings).
     * Cheat mode needs to be on for any cheats to trigger.
     *
     * @return true if cheat mode is on, false otherwise
     */
    bool IsCheatModeOn() const noexcept { return isCheatModeOn_; }

    // Classic cheats

    /** The classic ALT+1 through ALT+6 cheat which changes the game speed.
     *
     * @param speedIndex - 0 is normal, 1 is faster, 2 is even faster, etc.
     */
    void SetGameSpeed(uint8_t speedIndex);

    /** The classic F7 cheat.
     * Does not modify game state, merely tricks clients into revealing the whole map.
     * In the background, visibility is tracked as expected, i.e. if you reveal the map, send a scout and unreveal the
     * map, you will see what was scouted.
     */
    void ToggleAllVisible();
    bool IsAllVisible() const noexcept { return isAllVisible_; }

    /** The classic build headquarters cheat.
     * This function is used to check if the cheat building can be placed at the chosen point when opening the activity
     * window.
     *
     * @param mp - The map point where the user clicked to open the activity window.
     * @return true if the building can be placed, false otherwise
     */
    bool CanPlaceCheatBuilding(const MapPoint& mp) const;
    /** The classic build headquarters cheat.
     * This function is used to place the cheat building at the chosen point.
     * The building is immediately fully built, there is no need to build it using settlers.
     *
     * @param mp - The map point at which to place the building.
     * @param player - The player to whom the building should belong.
     */
    void PlaceCheatBuilding(const MapPoint& mp, const GamePlayer& player);

    // RTTR cheats

    /** Shares control of the (human) user's country with the AI. Both the user and the AI retain full control of the
     * country, so the user can observe what the AI does or "cooperate" with it.
     */
    void ToggleHumanAIPlayer();

    enum class ResourceRevealMode
    {
        Nothing,
        Ores,
        Fish,
        Water
    };
    /** Tells clients which resources to reveal:
     * Nothing - reveal nothing
     * Ores - reveal ores
     * Fish - reveal ores and fish
     * Water - reveal ores, fish and water
     */
    ResourceRevealMode GetResourceRevealMode() const;
    void ToggleResourceRevealMode();

    /** Destroys all buildings of AI players.
     */
    void DestroyAllAI();

private:
    std::unique_ptr<CheatKeyTracker> keyTracker_;
    GameWorldBase& world_;
    bool isCheatModeOn_ = false;
    bool isAllVisible_ = false;
    ResourceRevealMode resourceRevealMode_ = ResourceRevealMode::Nothing;
};
