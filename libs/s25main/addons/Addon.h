// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#pragma once

#ifndef ADDON_H_INCLUDED
#define ADDON_H_INCLUDED

#include "const_addons.h"
#include <string>
#include <utility>

class Window;

/**
 *  Addon baseclass
 */
class Addon
{
public:
    Addon(const AddonId id, AddonGroup groups, std::string name, std::string description, unsigned default_status)
        : id_(id), groups_(groups), name_(std::move(name)), description_(std::move(description)), defaultStatus_(default_status)
    {}
    virtual ~Addon() = default;

    virtual void hideGui(Window* window, unsigned id) const;
    virtual void createGui(Window* window, unsigned id, unsigned short& y, bool readonly, unsigned status) const;
    virtual void setGuiStatus(Window* /*window*/, unsigned /*id*/, unsigned /*status*/) const = 0;
    virtual unsigned getGuiStatus(Window* /*window*/, unsigned /*id*/, bool& failed) const = 0;

    AddonId getId() const { return id_; }
    AddonGroup getGroups() const { return groups_; }
    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }
    unsigned getDefaultStatus() const { return defaultStatus_; }
    virtual unsigned getNumOptions() const = 0;

private:
    AddonId id_;
    AddonGroup groups_;
    std::string name_;
    std::string description_;
    unsigned defaultStatus_;
};

#endif // !ADDON_H_INCLUDED
