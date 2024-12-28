/*
 * Copyright (c) 2023, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ELFWidget.h"
#include <LibCore/System.h>
#include <LibFileSystemAccessClient/Client.h>
#include <LibGUI/Action.h>
#include <LibGUI/Application.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Window.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio recvfd sendfd rpath unix"));

    auto app = TRY(GUI::Application::create(arguments));

    TRY(Core::System::unveil("/bin/ELFExplorer", "r")); // fixme: remove
    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil("/tmp/session/%sid/portal/config", "rw"));
    TRY(Core::System::unveil("/tmp/session/%sid/portal/filesystemaccess", "rw"));
    TRY(Core::System::unveil(nullptr, nullptr));

    // Window
    auto app_icon = TRY(GUI::Icon::try_create_default_icon("filetype-executable"sv)); // FIXME: Create ELFExplorer icon
    auto window = GUI::Window::construct();
    window->set_title("ELF Explorer");
    window->set_icon(app_icon.bitmap_for_size(16));
    window->restore_size_and_position("ELFExplorer"sv, "Window"sv, { { 640, 480 } });
    window->save_size_and_position_on_close("ELFExplorer"sv, "Window"sv);

    // ELF widget
    auto elf_widget = TRY(ELFExplorer::ELFWidget::create());
    window->set_main_widget(elf_widget);

    // Main menu actions
    auto file_menu = window->add_menu("&File"_string);
    file_menu->add_action(GUI::CommonActions::make_open_action([&](auto&) {
        FileSystemAccessClient::OpenFileOptions options {
            .allowed_file_types = { { GUI::FileTypeFilter { "Executable Files", { { "", ".elf" } } },
                GUI::FileTypeFilter { "Library Files", { { ".so", ".so.serenity" } } },
                GUI::FileTypeFilter::all_files() } },
        };
        auto response = FileSystemAccessClient::Client::the().open_file(window, options);
        if (response.is_error())
            return;

        auto file = response.release_value();
        (void)elf_widget->open_file(file.filename());
    }));
    file_menu->add_separator();
    file_menu->add_action(GUI::CommonActions::make_quit_action([](auto&) { GUI::Application::the()->quit(); }));

    auto help_menu = window->add_menu("&Help"_string);
    help_menu->add_action(GUI::CommonActions::make_command_palette_action(window));
    help_menu->add_action(GUI::CommonActions::make_about_action("ELF Explorer"_string, app_icon, window));

    // FIXME: remove
    MUST(elf_widget->open_file("/bin/ELFExplorer"));

    window->show();

    return app->exec();
}
