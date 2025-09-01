/*
 * Copyright (c) 2025, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UsersTab.h"
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/TabWidget.h>
#include <LibGUI/Window.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio recvfd sendfd rpath wpath cpath unix proc exec"));

    auto app = TRY(GUI::Application::create(arguments));

    TRY(Core::System::unveil("/bin/useradd", "x"));
    TRY(Core::System::unveil("/bin/userdel", "x"));
    TRY(Core::System::unveil("/home", "rwc"));
    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil("/etc", "rwc"));
    TRY(Core::System::unveil(nullptr, nullptr));

    // FIXME: Add icon
    auto window = GUI::Window::construct();
    window->set_title("Users Settings");
    window->resize(500, 400);

    // Root
    auto root = window->set_main_widget<GUI::TabWidget>();

    // Users Tab
    auto users_tab = TRY(UsersSettings::UsersTab::try_create());
    root->add_tab(users_tab, "Users"_string);

    window->show();
    return app->exec();
}
