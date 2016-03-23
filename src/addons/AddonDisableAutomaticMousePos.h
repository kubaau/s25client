// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#ifndef ADDONDISABLEAUTOMATICMOUSEPOS_H_INCLUDED
#define ADDONDISABLEAUTOMATICMOUSEPOS_H_INCLUDED

#pragma once

#include "Addons.h"
#include "../mygettext/src/mygettext.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon that allows disabling the automatic mouse cursor positioning on modals, so that e.g. local split-screen multiplayer with xinput is possible
 *
 *  @author Jonathan Reisdorf
 */
class AddonDisableAutomaticMousePos : public AddonBool
{
    public:
        AddonDisableAutomaticMousePos() : AddonBool(ADDON_DISABLE_AUTOMATIC_MOUSE_POS,
                                             ADDONGROUP_OTHER,
                                             gettext_noop("Disable Automatic Mouse Positioning"),
                                             gettext_noop("Disable automatic mouse positioning for modals\n"
                                                     "Useful for local split-screen multiplayer, e.g. with xinput"),
                                             0
                                            )
        {
        }
};

#endif // !ADDONDISABLEAUTOMATICMOUSEPOS_H_INCLUDED
