/*
 * Copyright (c) 2022, Tim Flynn <trflynn89@serenityos.org>
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DateTimeTab.h"
#include "TaskbarClockTab.h"
#include "TimeZoneTab.h"
#include <LibConfig/Client.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/Icon.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/SettingsWindow.h>
#include <LibMain/Main.h>
#include <unistd.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio rpath recvfd sendfd unix proc exec settime"));

    auto app = TRY(GUI::Application::create(arguments));

    Config::pledge_domains({ "Calendar", "Taskbar", "TimeServer" });

    // Ensure clock config defaults are always present so the taskbar and other readers get a real value.
    if (Config::read_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, ""sv).is_empty())
        Config::write_string("Taskbar"sv, "Clock"sv, "TimeFormat"sv, "%T"sv);
    if (Config::read_string("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, ""sv).is_empty())
        Config::write_bool("Taskbar"sv, "Clock"sv, "Use24HourClock"sv, true);

    StringView selected_tab;
    Core::ArgsParser args_parser;
    args_parser.add_option(selected_tab, "Tab, one of 'date-time', 'time-zone' or 'taskbar-clock'", "open-tab", 't', "tab");
    args_parser.parse(arguments);

    TRY(Core::System::pledge("stdio rpath recvfd sendfd proc exec settime"));
    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil("/bin/timezone", "x"));
    TRY(Core::System::unveil("/etc/timezone", "r"));
    TRY(Core::System::unveil(nullptr, nullptr));

    auto app_icon = GUI::Icon::default_icon("app-analog-clock"sv); // FIXME: Create a ClockSettings icon.

    auto window = TRY(GUI::SettingsWindow::create("Clock Settings", GUI::SettingsWindow::ShowDefaultsButton::Yes));

    if (getuid() != 0) {
        GUI::MessageBox::show_error(window, "Clock Settings must be run as root in order to set the system time."sv);
        return Error::from_string_view("Clock Settings must be run as root in order to set the system time."sv);
    }

    (void)TRY(window->add_tab<DateTimeTab>("Date & Time"_string, "date-time"sv));
    auto timezone_widget = TRY(TimeZoneTab::create());
    TRY(window->add_tab(timezone_widget, "Time Zone"_string, "time-zone"sv));
    (void)TRY(window->add_tab<TaskbarClockTab>("Taskbar Clock"_string, "taskbar-clock"sv));

    window->set_icon(app_icon.bitmap_for_size(16));
    window->resize(540, 570);
    window->set_active_tab(selected_tab);

    window->show();
    return app->exec();
}
