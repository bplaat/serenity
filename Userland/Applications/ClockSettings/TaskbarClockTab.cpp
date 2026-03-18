/*
 * Copyright (c) 2022, cflip <cflip@cflip.net>
 * Copyright (c) 2022, Sam Atkins <atkinssj@serenityos.org>
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "TaskbarClockTab.h"
#include <Applications/ClockSettings/TaskbarClockTabGML.h>
#include <LibConfig/Client.h>
#include <LibCore/DateTime.h>
#include <LibGUI/Label.h>
#include <LibGUI/RadioButton.h>
#include <LibGUI/TextBox.h>

ErrorOr<NonnullRefPtr<TaskbarClockTab>> TaskbarClockTab::try_create()
{
    auto widget = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) TaskbarClockTab()));
    TRY(widget->setup());
    return widget;
}

static ByteString format_for_mode(bool hhmm, bool use_24h)
{
    if (use_24h)
        return hhmm ? "%R" : "%T";
    return hhmm ? "%I:%M %p" : "%r";
}

ErrorOr<void> TaskbarClockTab::setup()
{
    TRY(load_from_gml(taskbar_clock_tab_gml));

    m_hhmm_radio = *find_descendant_of_type_named<GUI::RadioButton>("hhmm_radio");
    m_hhmmss_radio = *find_descendant_of_type_named<GUI::RadioButton>("hhmmss_radio");
    auto& custom_radio = *find_descendant_of_type_named<GUI::RadioButton>("custom_radio");
    m_custom_format_input = *find_descendant_of_type_named<GUI::TextBox>("custom_format_input");
    m_clock_preview = *find_descendant_of_type_named<GUI::Label>("clock_preview");

    m_time_format = Config::read_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, "%T"sv);
    bool use_24h = Config::read_bool("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, true);

    m_custom_format_input->set_enabled(false);
    m_custom_format_input->on_change = [&] {
        m_time_format = m_custom_format_input->get_text();
        set_modified(true);
        update_clock_preview();
    };

    if (m_time_format == format_for_mode(true, use_24h)) {
        m_hhmm_radio->set_checked(true, GUI::AllowCallback::No);
    } else if (m_time_format == format_for_mode(false, use_24h)) {
        m_hhmmss_radio->set_checked(true, GUI::AllowCallback::No);
    } else {
        custom_radio.set_checked(true, GUI::AllowCallback::No);
        m_custom_format_input->set_enabled(true);
    }
    m_custom_format_input->set_text(m_time_format);

    auto on_hhmm_checked = [&](bool checked) {
        if (!checked)
            return;
        m_custom_format_input->set_enabled(false);
        bool current_24h = Config::read_bool("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, true);
        m_time_format = format_for_mode(true, current_24h);
        m_custom_format_input->set_text(m_time_format);
        set_modified(true);
        update_clock_preview();
    };
    auto on_hhmmss_checked = [&](bool checked) {
        if (!checked)
            return;
        m_custom_format_input->set_enabled(false);
        bool current_24h = Config::read_bool("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, true);
        m_time_format = format_for_mode(false, current_24h);
        m_custom_format_input->set_text(m_time_format);
        set_modified(true);
        update_clock_preview();
    };

    m_hhmm_radio->on_checked = move(on_hhmm_checked);
    m_hhmmss_radio->on_checked = move(on_hhmmss_checked);

    custom_radio.on_checked = [&](bool checked) {
        if (!checked)
            return;
        m_custom_format_input->set_enabled(true);
        set_modified(true);
    };

    m_clock_preview_update_timer = Core::Timer::create_repeating(1000, [&]() {
        update_clock_preview();
    });
    m_clock_preview_update_timer->start();
    update_clock_preview();

    return {};
}

void TaskbarClockTab::apply_settings()
{
    Config::write_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, m_custom_format_input->text());
}

void TaskbarClockTab::reset_default_values()
{
    bool use_24h = Config::read_bool("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, true);
    m_time_format = format_for_mode(false, use_24h);
    m_hhmmss_radio->set_checked(true);
    m_custom_format_input->set_text(m_time_format);
}

void TaskbarClockTab::update_clock_preview()
{
    m_clock_preview->set_text(Core::DateTime::now().to_string(m_time_format).release_value_but_fixme_should_propagate_errors());
}
