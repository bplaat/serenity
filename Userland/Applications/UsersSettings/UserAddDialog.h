/*
 * Copyright (c) 2023, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGUI/Widget.h>

namespace UsersSettings {

class UserAddDialog final : public GUI::Widget {
    C_OBJECT(UserAddDialog)
public:
    static ErrorOr<NonnullRefPtr<UserAddDialog>> try_create();
};

}
