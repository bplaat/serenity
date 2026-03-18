/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DateTimeTab.h"
#include <Applications/ClockSettings/DateTimeTabGML.h>
#include <LibConfig/Client.h>
#include <LibCore/DateTime.h>
#include <LibCore/System.h>
#include <LibCore/Timer.h>
#include <LibGUI/Button.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/DatePicker.h>
#include <LibGUI/ItemListModel.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/RadioButton.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/TimePicker.h>
#include <time.h>

struct NtpServer {
    StringView label;
    StringView host;
};

static constexpr Array ntp_servers = {
    NtpServer { "Google (time.google.com)"sv, "time.google.com"sv },
    NtpServer { "Apple (time.apple.com)"sv, "time.apple.com"sv },
    NtpServer { "Cloudflare (time.cloudflare.com)"sv, "time.cloudflare.com"sv },
    NtpServer { "Windows (time.windows.com)"sv, "time.windows.com"sv },
    NtpServer { "Facebook (time.facebook.com)"sv, "time.facebook.com"sv },
    NtpServer { "NTP Pool (pool.ntp.org)"sv, "pool.ntp.org"sv },
    NtpServer { "NIST (time.nist.gov)"sv, "time.nist.gov"sv },
};

ErrorOr<NonnullRefPtr<DateTimeTab>> DateTimeTab::try_create()
{
    auto widget = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) DateTimeTab()));
    TRY(widget->setup());
    return widget;
}

ErrorOr<void> DateTimeTab::setup()
{
    TRY(load_from_gml(date_time_tab_gml));

    m_current_time_label = *find_descendant_of_type_named<GUI::Label>("current_time_label");
    m_date_button = *find_descendant_of_type_named<GUI::Button>("date_button");
    m_time_button = *find_descendant_of_type_named<GUI::Button>("time_button");
    m_auto_sync_checkbox = *find_descendant_of_type_named<GUI::CheckBox>("auto_sync_checkbox");
    m_ntp_controls = *find_descendant_of_type_named<GUI::Widget>("ntp_controls");
    m_ntp_server_combobox = *find_descendant_of_type_named<GUI::ComboBox>("ntp_server_combobox");
    m_custom_ntp_server_row = *find_descendant_of_type_named<GUI::Widget>("custom_ntp_server_row");
    m_custom_ntp_server_input = *find_descendant_of_type_named<GUI::TextBox>("custom_ntp_server_input");
    m_use_24hour_radio = *find_descendant_of_type_named<GUI::RadioButton>("use_24hour_radio");

    load_current_time();

    m_date_button->on_click = [&](unsigned) {
        auto new_date = GUI::DatePicker::show(window(), "Select Date"_string, m_loaded_time);
        if (!new_date.has_value())
            return;
        m_loaded_time.set_date(*new_date);
        m_date_button->set_text(MUST(m_loaded_time.to_string("%B %e, %Y"sv)));
        set_modified(true);
    };

    m_time_button->on_click = [&](unsigned) {
        bool use_24h = m_use_24hour_radio->is_checked();
        auto new_time = GUI::TimePicker::show(window(), "Select Time"_string, m_loaded_time, use_24h);
        if (!new_time.has_value())
            return;
        m_loaded_time.set_time(m_loaded_time.year(), m_loaded_time.month(), m_loaded_time.day(),
            new_time->hour(), new_time->minute(), new_time->second());
        auto time_fmt = use_24h ? "%H:%M:%S"sv : "%I:%M:%S %p"sv;
        m_time_button->set_text(MUST(m_loaded_time.to_string(time_fmt)));
        set_modified(true);
    };

    m_auto_sync_checkbox->set_checked(Config::read_bool("TimeServer"sv, "Client"sv, "Enabled"sv, false), GUI::AllowCallback::No);

    for (auto& server : ntp_servers)
        m_ntp_server_labels.append(MUST(String::from_utf8(server.label)));
    m_ntp_server_labels.append("Custom..."_string);
    m_ntp_server_combobox->set_model(GUI::ItemListModel<String>::create(m_ntp_server_labels));
    m_ntp_server_combobox->set_only_allow_values_from_model(true);

    auto saved_host = Config::read_string("TimeServer"sv, "Client"sv, "ServerAddress"sv, "time.google.com"sv);
    size_t server_index = ntp_servers.size();
    for (size_t i = 0; i < ntp_servers.size(); ++i) {
        if (ntp_servers[i].host == saved_host) {
            server_index = i;
            break;
        }
    }
    m_ntp_server_combobox->set_selected_index(server_index, GUI::AllowCallback::No);
    if (server_index == ntp_servers.size())
        m_custom_ntp_server_input->set_text(saved_host);

    m_auto_sync_checkbox->on_checked = [&](bool) {
        update_manual_controls_enabled();
        set_modified(true);
    };

    m_ntp_server_combobox->on_change = [&](auto, auto) {
        update_ntp_server_controls();
        set_modified(true);
    };

    m_custom_ntp_server_input->on_change = [&] { set_modified(true); };

    bool use_24h = Config::read_bool("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, true);
    m_use_24hour_radio->set_checked(use_24h, GUI::AllowCallback::No);
    m_use_24hour_radio->on_checked = [&](bool) { set_modified(true); };
    find_descendant_of_type_named<GUI::RadioButton>("use_12hour_radio")->on_checked = [&](bool) { set_modified(true); };

    update_manual_controls_enabled();
    update_ntp_server_controls();

    m_clock_update_timer = Core::Timer::create_repeating(1000, [&]() {
        auto fmt = m_use_24hour_radio->is_checked() ? "%B %e, %Y %H:%M:%S"sv : "%B %e, %Y %I:%M:%S %p"sv;
        m_current_time_label->set_text(MUST(Core::DateTime::now().to_string(fmt)));
    });
    m_clock_update_timer->start();

    return {};
}

void DateTimeTab::load_current_time()
{
    m_loaded_time = Core::DateTime::now();
    bool use_24h = Config::read_bool("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, true);
    auto time_fmt = use_24h ? "%H:%M:%S"sv : "%I:%M:%S %p"sv;
    auto datetime_fmt = use_24h ? "%B %e, %Y %H:%M:%S"sv : "%B %e, %Y %I:%M:%S %p"sv;
    m_date_button->set_text(MUST(m_loaded_time.to_string("%B %e, %Y"sv)));
    m_time_button->set_text(MUST(m_loaded_time.to_string(time_fmt)));
    m_current_time_label->set_text(MUST(m_loaded_time.to_string(datetime_fmt)));
}

void DateTimeTab::update_manual_controls_enabled()
{
    bool auto_sync = m_auto_sync_checkbox->is_checked();
    m_date_button->set_enabled(!auto_sync);
    m_time_button->set_enabled(!auto_sync);
    m_ntp_controls->set_enabled(auto_sync);
}

void DateTimeTab::apply_settings()
{
    Config::write_bool("TimeServer"sv, "Client"sv, "Enabled"sv, m_auto_sync_checkbox->is_checked());

    auto selected_index = m_ntp_server_combobox->selected_index();
    ByteString server_host = selected_index < ntp_servers.size()
        ? ByteString(ntp_servers[selected_index].host)
        : m_custom_ntp_server_input->get_text();
    Config::write_string("TimeServer"sv, "Client"sv, "ServerAddress"sv, server_host);

    bool use_24h = m_use_24hour_radio->is_checked();
    Config::write_bool("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, use_24h);

    // Keep TimeFormat in sync with the 12/24h preference, mapping known formats to their equivalent.
    // Custom formats are left unchanged.
    auto current_format = Config::read_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, "%T"sv);
    if (current_format == "%R" || current_format == "%I:%M %p")
        Config::write_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, use_24h ? "%R"sv : "%I:%M %p"sv);
    else if (current_format == "%T" || current_format == "%r")
        Config::write_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, use_24h ? "%T"sv : "%r"sv);

    if (!m_auto_sync_checkbox->is_checked()) {
        struct timespec ts;
        ts.tv_sec = m_loaded_time.timestamp();
        ts.tv_nsec = 0;
        if (auto result = Core::System::clock_settime(CLOCK_REALTIME, &ts); result.is_error())
            GUI::MessageBox::show_error(window(), MUST(String::formatted("Failed to set system time: {}", result.error())));
    }
}

void DateTimeTab::reset_default_values()
{
    load_current_time();
    m_auto_sync_checkbox->set_checked(true);
    m_ntp_server_combobox->set_selected_index(0);
    m_custom_ntp_server_input->set_text(""sv);
    m_use_24hour_radio->set_checked(true);
    update_manual_controls_enabled();
    update_ntp_server_controls();
}

void DateTimeTab::update_ntp_server_controls()
{
    bool is_custom = m_ntp_server_combobox->selected_index() == ntp_servers.size();
    m_custom_ntp_server_row->set_visible(is_custom);
}
