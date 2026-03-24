/*
 * Copyright (c) 2025, Sönke Holz <soenke.holz@serenityos.org>
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/TypedTransfer.h>
#include <Kernel/Devices/Input/HID/ConsumerControlDriver.h>
#include <Kernel/Devices/Input/HID/Definitions.h>
#include <Kernel/Devices/Input/HID/Device.h>
#include <Kernel/Devices/Input/Management.h>
#include <LibHID/ReportParser.h>

namespace Kernel::HID {

ConsumerControlDriver::~ConsumerControlDriver()
{
    InputManagement::the().detach_standalone_input_device(*m_keyboard_device);
}

ErrorOr<NonnullRefPtr<ConsumerControlDriver>> ConsumerControlDriver::create(Device const& device, ::HID::ApplicationCollection const& application_collection)
{
    auto keyboard_device = TRY(::KeyboardDevice::try_to_initialize());
    auto handler = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) ConsumerControlDriver(device, application_collection, move(keyboard_device))));
    InputManagement::the().attach_standalone_input_device(*handler->m_keyboard_device);
    return handler;
}

ConsumerControlDriver::ConsumerControlDriver(Device const& device, ::HID::ApplicationCollection const& application_collection, NonnullRefPtr<::KeyboardDevice> keyboard_device)
    : ApplicationCollectionDriver(device, application_collection)
    , m_keyboard_device(move(keyboard_device))
{
}

void ConsumerControlDriver::handle_consumer_change_event(u16 usage_id, bool value)
{
    struct ConsumerKeyEntry {
        u16 usage_id;
        KeyCode key_code;
    };

    static constexpr auto consumer_key_map = to_array<ConsumerKeyEntry>({
        { to_underlying(Usage::ScanNextTrack) & 0xffff, Key_NextTrack },
        { to_underlying(Usage::ScanPreviousTrack) & 0xffff, Key_PreviousTrack },
        { to_underlying(Usage::Stop) & 0xffff, Key_Stop },
        { to_underlying(Usage::PlayPause) & 0xffff, Key_PlayPause },
        { to_underlying(Usage::Mute) & 0xffff, Key_Mute },
        { to_underlying(Usage::VolumeIncrement) & 0xffff, Key_VolumeUp },
        { to_underlying(Usage::VolumeDecrement) & 0xffff, Key_VolumeDown },
    });

    KeyCode key_code = Key_Invalid;
    for (auto const& entry : consumer_key_map) {
        if (usage_id == entry.usage_id) {
            key_code = entry.key_code;
            break;
        }
    }

    if (key_code == Key_Invalid) {
        dbgln_if(HID_DEBUG, "HID: Unknown Consumer Page Usage ID: {:#x}", usage_id);
        return;
    }

    KeyEvent key_event {
        .key = key_code,
        .map_entry_index = 0xff,
        .scancode = usage_id,
        .flags = value ? static_cast<u8>(Is_Press) : static_cast<u8>(0),
    };
    m_keyboard_device->handle_input_event(key_event);
}

ErrorOr<void> ConsumerControlDriver::on_report(ReadonlyBytes report_data)
{
    decltype(m_key_state) new_key_state {};

    TRY(::HID::parse_input_report(m_device.report_descriptor(), m_application_collection, report_data, [&new_key_state](::HID::Field const& field, i64 value) -> ErrorOr<IterationDecision> {
        u32 usage = 0;
        if (field.is_array) {
            if (!field.usage_minimum.has_value())
                return Error::from_errno(ENOTSUP);
            usage = value + field.usage_minimum.value();
            value = 1;
        } else {
            usage = field.usage.value_or(0);
        }

        if ((usage >> 16) != to_underlying(UsagePage::Consumer)) {
            dbgln_if(HID_DEBUG, "HID: Unknown Consumer Application Collection Usage: {:#x}", usage);
            return IterationDecision::Continue;
        }

        u16 usage_id = usage & 0xffff;

        if (usage_id >= key_state_bitmap_size_in_bits) {
            dbgln_if(HID_DEBUG, "HID: Unknown Consumer Page Usage ID: {:#x}", usage_id);
            return IterationDecision::Continue;
        }

        if (value != 0)
            new_key_state[usage_id / key_state_bitmap_bits_per_element] |= 1zu << (usage_id % key_state_bitmap_bits_per_element);

        return IterationDecision::Continue;
    }));

    // FIXME: Optimize this.
    for (size_t i = 0; i < key_state_bitmap_size_in_bits; i++) {
        auto old_bitmap_element = m_key_state[i / key_state_bitmap_bits_per_element];
        auto new_bitmap_element = new_key_state[i / key_state_bitmap_bits_per_element];

        bool old_value = old_bitmap_element & (1zu << (i % key_state_bitmap_bits_per_element));
        bool new_value = new_bitmap_element & (1zu << (i % key_state_bitmap_bits_per_element));

        if (old_value != new_value)
            handle_consumer_change_event(i, new_value);
    }

    AK::TypedTransfer<KeyStateBitmapElement>::copy(m_key_state.data(), new_key_state.data(), m_key_state.size());

    return {};
}

}
