// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "TransmitSettingsIgwAdapter.h"
#include <vector>

class GameWorldViewer;

class iwBuildOrder final : public TransmitSettingsIgwAdapter
{
    const GameWorldViewer& gwv;

    std::vector<BuildingType> pendingBuildOrder;
    bool useCustomBuildOrder;

public:
    iwBuildOrder(const GameWorldViewer& gwv);

private:
    /// Updates the control elements with the pending build order
    void UpdateSettings() override;
    /// Sendet veränderte Einstellungen (an den Client), falls sie verändert wurden
    void TransmitSettings() override;

    void Msg_ListSelectItem(unsigned ctrl_id, int selection) override;
    void Msg_ButtonClick(unsigned ctrl_id) override;
    void Msg_ComboSelectItem(unsigned ctrl_id, unsigned selection) override;

    void fillBuildOrder();
};
