/*
 * Copyright (c) 2022, cflip <cflip@cflip.net>
 * Copyright (c) 2022, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ClockSettingsWidget.h"
#include <Applications/ClockSettings/ClockSettingsWidgetGML.h>
#include <LibConfig/Client.h>
#include <LibCore/DateTime.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/Label.h>
#include <LibGUI/RadioButton.h>
#include <LibGUI/TextBox.h>
#include <LibLocale/DateTimeFormat.h>
#include <LibLocale/Locale.h>

constexpr auto time_format_12h = "%I:%M %p"sv;
constexpr auto time_format_12h_seconds = "%r"sv;
constexpr auto time_format_24h = "%R"sv;
constexpr auto time_format_24h_seconds = "%T"sv;

static bool locale_uses_24h(StringView tag)
{
    auto cycle = Locale::get_default_regional_hour_cycle(tag);
    if (cycle.has_value())
        return *cycle == Locale::HourCycle::H23 || *cycle == Locale::HourCycle::H24;
    // Fallback when CLDR data is not compiled in:
    return tag != "en-US"sv && tag != "en-GB"sv && tag != "en-AU"sv
        && tag != "en-CA"sv && tag != "es-MX"sv;
}

ErrorOr<NonnullRefPtr<ClockSettingsWidget>> ClockSettingsWidget::try_create()
{
    auto widget = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) ClockSettingsWidget()));
    TRY(widget->setup());
    return widget;
}

ErrorOr<void> ClockSettingsWidget::setup()
{
    TRY(load_from_gml(clock_settings_widget_gml));

    m_locale_default_radio = *find_descendant_of_type_named<GUI::RadioButton>("locale_default_radio");
    auto& custom_radio = *find_descendant_of_type_named<GUI::RadioButton>("custom_radio");
    m_show_seconds_checkbox = *find_descendant_of_type_named<GUI::CheckBox>("seconds_checkbox");
    m_clock_preview = *find_descendant_of_type_named<GUI::Label>("clock_preview");

    // Determine the locale-based default format.
    auto region = Config::read_string("Locale"sv, "Locale"sv, "Region"sv, "en-US"sv);
    m_locale_uses_24h = locale_uses_24h(region);

    // Read any explicit override stored in Config (empty = use locale-derived format).
    m_time_format = Config::read_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, ""sv);

    m_custom_format_input = *find_descendant_of_type_named<GUI::TextBox>("custom_format_input");
    m_custom_format_input->set_text(m_time_format);
    m_custom_format_input->set_enabled(false);
    m_custom_format_input->on_change = [&] {
        m_time_format = m_custom_format_input->get_text();
        set_modified(true);
        update_clock_preview();
    };

    // Detect current mode.
    bool is_custom = !m_time_format.is_empty()
        && m_time_format != time_format_12h
        && m_time_format != time_format_12h_seconds
        && m_time_format != time_format_24h
        && m_time_format != time_format_24h_seconds;

    if (is_custom) {
        custom_radio.set_checked(true, GUI::AllowCallback::No);
        m_show_seconds_checkbox->set_enabled(false);
        m_custom_format_input->set_enabled(true);
    } else {
        m_locale_default_radio->set_checked(true, GUI::AllowCallback::No);
        bool has_seconds = m_time_format == time_format_12h_seconds || m_time_format == time_format_24h_seconds;
        m_show_seconds_checkbox->set_checked(has_seconds, GUI::AllowCallback::No);
        // If the format was empty (no override), default seconds to true.
        if (m_time_format.is_empty())
            m_show_seconds_checkbox->set_checked(true, GUI::AllowCallback::No);
    }

    m_locale_default_radio->on_checked = [&](bool checked) {
        if (!checked)
            return;
        m_show_seconds_checkbox->set_enabled(true);
        m_custom_format_input->set_enabled(false);
        set_modified(true);
        update_time_format_string();
    };

    m_show_seconds_checkbox->on_checked = [&](bool) {
        set_modified(true);
        update_time_format_string();
    };

    custom_radio.on_checked = [&](bool checked) {
        if (!checked)
            return;
        m_show_seconds_checkbox->set_enabled(false);
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

void ClockSettingsWidget::apply_settings()
{
    Config::write_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, m_custom_format_input->text());
}

void ClockSettingsWidget::reset_default_values()
{
    m_locale_default_radio->set_checked(true);
    m_show_seconds_checkbox->set_checked(true);
    // Write empty string so Taskbar derives format from locale.
    Config::write_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, ""sv);
}

void ClockSettingsWidget::update_time_format_string()
{
    bool show_seconds = m_show_seconds_checkbox->is_checked();
    if (m_locale_uses_24h)
        m_time_format = (show_seconds ? time_format_24h_seconds : time_format_24h);
    else
        m_time_format = (show_seconds ? time_format_12h_seconds : time_format_12h);
    m_custom_format_input->set_text(m_time_format);
    update_clock_preview();
}

void ClockSettingsWidget::update_clock_preview()
{
    m_clock_preview->set_text(Core::DateTime::now().to_string(m_time_format.is_empty() ? (m_locale_uses_24h ? time_format_24h_seconds : time_format_12h_seconds) : StringView(m_time_format)).release_value_but_fixme_should_propagate_errors());
}
