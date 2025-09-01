/*
 * Copyright (c) 2025, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UsersTab.h"
#include "UserAddDialog.h"
#include <AK/String.h>
#include <AK/StringView.h>
#include <Applications/UsersSettings/UserAddDialog.h>
#include <LibCore/Account.h>
#include <LibCore/Command.h>
#include <LibCore/SecretString.h>
#include <LibCore/System.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/ItemListModel.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/TextBox.h>
#include <grp.h>
#include <unistd.h>

namespace UsersSettings {

ErrorOr<void> UsersTab::initialize()
{
    m_users_list = *find_descendant_of_type_named<GUI::ListView>("users_list");
    m_add_button = *find_descendant_of_type_named<GUI::Button>("add_button");
    m_delete_button = *find_descendant_of_type_named<GUI::Button>("delete_button");

    TRY(refresh_users());
    m_users_list->set_model(*GUI::ItemListModel<String>::create(m_usernames));

    m_add_button->on_click = [this](unsigned) {
        MUST(add_user());
    };

    m_delete_button->on_click = [this](unsigned) {
        auto index = m_users_list->selection().first();
        if (!index.is_valid())
            return;
        MUST(delete_user(m_usernames[index.row()].bytes_as_string_view()));
    };

    return {};
}

ErrorOr<void> UsersTab::refresh_users()
{
    auto accounts = TRY(Core::Account::all());
    m_usernames.clear();
    for (auto& account : accounts) {
        m_usernames.append(TRY(String::from_byte_string(account.username())));
    }
    return {};
}

ErrorOr<void> UsersTab::add_user()
{
    auto add_dialog = TRY(GUI::Dialog::try_create(window()));
    add_dialog->set_title("Add user");
    add_dialog->set_resizable(false);

    auto widget = TRY(UserAddDialog::try_create());
    add_dialog->set_main_widget(widget);

    auto& account_type_combobox = *widget->find_descendant_of_type_named<GUI::ComboBox>("account_type_combobox");
    account_type_combobox.set_model(*GUI::ItemListModel<String>::create({ "Standard"_string, "Administrator"_string }));

    auto& full_name_textbox = *widget->find_descendant_of_type_named<GUI::TextBox>("full_name_textbox");
    auto& username_textbox = *widget->find_descendant_of_type_named<GUI::TextBox>("username_textbox");
    auto& password_textbox = *widget->find_descendant_of_type_named<GUI::PasswordBox>("password_textbox");

    auto& ok_button = *widget->find_descendant_of_type_named<GUI::Button>("ok_button");
    ok_button.on_click = [&](auto) {
        // Create user
        auto full_name = MUST(String::from_byte_string(full_name_textbox.text()));
        auto username = MUST(String::from_byte_string(username_textbox.text()));
        auto password = Core::SecretString::take_ownership(password_textbox.text().to_byte_buffer());
        if (!full_name.is_empty()) {
            MUST(Core::command("useradd"sv, { "--create-home"sv, "--gecos"sv, full_name.bytes_as_string_view(), username.bytes_as_string_view() }, {}));
        } else {
            MUST(Core::command("useradd"sv, { "--create-home"sv, username.bytes_as_string_view() }, {}));
        }

        // Update account
        auto account = MUST(Core::Account::from_name(username));
        auto gids = account.extra_gids();
        if (account_type_combobox.selected_index() == 0)
            gids.append(getgrnam("wheel")->gr_gid);
        gids.append(getgrnam("window")->gr_gid);
        gids.append(getgrnam("audio")->gr_gid);
        gids.append(getgrnam("lookup")->gr_gid);
        account.set_extra_gids(gids);
        MUST(account.set_password(password));
        MUST(account.sync());

        // Create home folders
        Vector<StringView> well_known_dirs = { "Desktop"sv, "Documents"sv, "Downloads"sv, "Music"sv, "Pictures"sv, "Videos"sv };
        for (auto& dir : well_known_dirs) {
            auto path = MUST(String::formatted("/home/{}/{}", username, dir));
            MUST(Core::System::mkdir(path, 0755));
            MUST(Core::System::chown(path, account.uid(), account.gid()));
        }

        add_dialog->done(GUI::Dialog::ExecResult::OK);
        MUST(refresh_users());
    };
    ok_button.set_default(true);

    auto& cancel_button = *widget->find_descendant_of_type_named<GUI::Button>("cancel_button");
    cancel_button.on_click = [add_dialog](auto) {
        add_dialog->done(GUI::Dialog::ExecResult::Cancel);
    };

    add_dialog->exec();
    return {};
}

ErrorOr<void>
UsersTab::delete_user(StringView username)
{
    // Ask user for confirmation
    auto result = GUI::MessageBox::show(window(),
        TRY(String::formatted("Are you sure you want to delete user: {}?", username)),
        "Confirm user deletion"sv,
        GUI::MessageBox::Type::Warning,
        GUI::MessageBox::InputType::YesNo);
    if (result != GUI::MessageBox::ExecResult::Yes)
        return {};

    // Delete user
    TRY(Core::command("userdel"sv, { "--remove"sv, username }, {}));

    // Refresh users list
    TRY(refresh_users());
    return {};
}
}
