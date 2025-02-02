// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "GlobalGameSettings.h"
#include <vector>

struct JoinPlayerInfo;

/// Model of the game lobby (preparation when starting a map)
/// Not to be confused with the lobby (List of online games)
class GameLobby
{
public:
    GameLobby(bool isSavegame, bool isHost, unsigned numPlayers);
    ~GameLobby();

    JoinPlayerInfo& getPlayer(unsigned playerId);
    const JoinPlayerInfo& getPlayer(unsigned playerId) const;
    const std::vector<JoinPlayerInfo>& getPlayers() const { return players_; }
    unsigned getNumPlayers() const;
    void setNumPlayers(unsigned num);

    GlobalGameSettings& getSettings() { return ggs_; }
    const GlobalGameSettings& getSettings() const { return ggs_; }

    bool isSavegame() const { return isSavegame_; }
    bool isHost() const { return isHost_; }

private:
    /// Is this a savegame or a loaded game?
    bool isSavegame_;
    /// Is this for the host/game creator
    bool isHost_;
    std::vector<JoinPlayerInfo> players_;
    GlobalGameSettings ggs_;
};
