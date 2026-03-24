/*
 * Copyright (c) 2025, Sönke Holz <soenke.holz@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>

namespace Kernel::HID {

// https://usb.org/sites/default/files/hut1_6.pdf#chapter.3
enum class UsagePage : u16 {
    GenericDesktop = 0x01,
    KeyboardOrKeypad = 0x07,
    Button = 0x09,
    Consumer = 0x0c,
};

enum class Usage : u32 {
    // Generic Desktop Page (0x01)
    // https://usb.org/sites/default/files/hut1_6.pdf#chapter.4
    Mouse = 0x0001'0002,
    Keyboard = 0x0001'0006,
    X = 0x0001'0030,
    Y = 0x0001'0031,
    Wheel = 0x0001'0038,

    // Keyboard/Keypad Page (0x07)
    // https://usb.org/sites/default/files/hut1_6.pdf#chapter.10
    KeypadNumlock = 0x0007'0054,
    Keypad1 = 0x0007'0059,
    KeypadDot = 0x0007'0063,
    KeyboardLeftControl = 0x0007'00e0,
    KeyboardLeftShift = 0x0007'00e1,
    KeyboardLeftAlt = 0x0007'00e2,
    KeyboardLeftGUI = 0x0007'00e3,
    KeyboardRightControl = 0x0007'00e4,
    KeyboardRightShift = 0x0007'00e5,
    KeyboardRightAlt = 0x0007'00e6,
    KeyboardRightGUI = 0x0007'00e7,

    // Button Page (0x09)
    // https://usb.org/sites/default/files/hut1_6.pdf#chapter.12
    NoButtonPressed = 0x0009'0000,
    Button1 = 0x0009'0001,
    Button2 = 0x0009'0002,
    Button3 = 0x0009'0003,
    Button4 = 0x0009'0004,
    Button5 = 0x0009'0005,

    // Consumer Page (0x0c)
    // https://usb.org/sites/default/files/hut1_6.pdf#chapter.15
    ConsumerControl = 0x000c'0001,
    ScanNextTrack = 0x000c'00b5,
    ScanPreviousTrack = 0x000c'00b6,
    Stop = 0x000c'00b7,
    PlayPause = 0x000c'00cd,
    Mute = 0x000c'00e2,
    VolumeIncrement = 0x000c'00e9,
    VolumeDecrement = 0x000c'00ea,
    ACPan = 0x000c'0238,
};

}
