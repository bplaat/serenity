/*
 * Copyright (c) 2025, Sönke Holz <soenke.holz@serenityos.org>
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/Input/HID/ApplicationCollectionDriver.h>
#include <Kernel/Devices/Input/HID/Definitions.h>
#include <Kernel/Devices/Input/KeyboardDevice.h>

namespace Kernel::HID {

class ConsumerControlDriver final : public ApplicationCollectionDriver {
public:
    virtual ~ConsumerControlDriver() override;
    static ErrorOr<NonnullRefPtr<ConsumerControlDriver>> create(Device const&, ::HID::ApplicationCollection const&);

private:
    ConsumerControlDriver(Device const&, ::HID::ApplicationCollection const&, NonnullRefPtr<KeyboardDevice>);

    void handle_consumer_change_event(u16 usage_id, bool value);

    // ^ApplicationCollectionDriver
    virtual ErrorOr<void> on_report(ReadonlyBytes) override;

    NonnullRefPtr<KeyboardDevice> m_keyboard_device;

    // "Consumer Volume Decrement" (0x00EA) is the highest currently tracked Consumer Page Usage ID.
    static constexpr size_t key_state_bitmap_size_in_bits = (to_underlying(Usage::VolumeDecrement) & 0xffff) + 1;
    using KeyStateBitmapElement = FlatPtr;
    static constexpr size_t key_state_bitmap_bits_per_element = NumericLimits<KeyStateBitmapElement>::digits();
    static constexpr size_t key_state_bitmap_array_element_count = ceil_div(key_state_bitmap_size_in_bits, key_state_bitmap_bits_per_element);
    Array<KeyStateBitmapElement, key_state_bitmap_array_element_count> m_key_state {};
};

}
