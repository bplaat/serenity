/*
 * Copyright (c) 2024, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "RegionSettingsWidget.h"
#include <LibConfig/Client.h>
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/Icon.h>
#include <LibGUI/SettingsWindow.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio rpath recvfd sendfd unix"));

    auto app = TRY(GUI::Application::create(arguments));

    Config::pledge_domains({ "Locale", "Calendar", "Taskbar" });

    TRY(Core::System::pledge("stdio rpath recvfd sendfd"));
    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil(nullptr, nullptr));

    auto app_icon = GUI::Icon::default_icon("app-hello-world"sv);

    auto window = TRY(GUI::SettingsWindow::create("Region Settings", GUI::SettingsWindow::ShowDefaultsButton::Yes));
    (void)TRY(window->add_tab<RegionSettingsWidget>("Region"_string, "region"sv));
    window->set_icon(app_icon.bitmap_for_size(16));
    window->resize(430, 320);

    window->show();
    return app->exec();
}
