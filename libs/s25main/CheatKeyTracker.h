// Copyright (C) 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/circular_buffer.hpp>

class Cheats;
struct KeyEvent;

class CheatKeyTracker final
{
public:
    CheatKeyTracker(Cheats&);

    /** Tracks keyboard events related to cheats and triggers the actual cheats.
     * Calls related private methods of this class in order but returns at the first success (return true).
     *
     * @param ke - The keyboard event encountered.
     */
    void TrackKeyEvent(const KeyEvent& ke);

private:
    /** Tracks keyboard events related to cheats and triggers the actual cheats, but only tracks events of type
     * different than KeyType::Char (e.g. F-keys).
     *
     * @param ke - The keyboard event encountered.
     * @return true if keyboard event was NOT of type KeyType::Char, false otherwise
     */
    bool TrackSpecialKeyEvent(const KeyEvent& ke);

    /** Tracks keyboard events related to game speed cheats (ALT+1..ALT+6) and triggers the actual cheats.
     *
     * @param ke - The keyboard event encountered.
     * @return true if keyboard event was related to game speed cheats, false otherwise
     */
    bool TrackSpeedKeyEvent(const KeyEvent& ke);

    /** Tracks keyboard events related to cheats and triggers the actual cheats, but only tracks events of type
     * KeyType::Char (e.g. enabling cheat mode by typing "winter").
     *
     * @param ke - The keyboard event encountered.
     * @return always true
     */
    bool TrackCharKeyEvent(const KeyEvent& ke);

    Cheats& cheats_;
    boost::circular_buffer<char> lastChars_;
};
