/*
 * Copyright (c) 2025, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <AK/StringView.h>
#include <LibGUI/Button.h>
#include <LibGUI/ListView.h>
#include <LibGUI/Widget.h>

namespace UsersSettings {

class UsersTab final : public GUI::Widget {
    C_OBJECT(UsersTab)

public:
    static ErrorOr<NonnullRefPtr<UsersTab>> try_create();
    ErrorOr<void> initialize();

protected:
    UsersTab() = default;

private:
    ErrorOr<void> refresh_users();
    ErrorOr<void> add_user();
    ErrorOr<void> delete_user(StringView username);

    RefPtr<GUI::ListView> m_users_list;
    Vector<String> m_usernames;
    RefPtr<GUI::Button> m_add_button;
    RefPtr<GUI::Button> m_delete_button;
};

}
