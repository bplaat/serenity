/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Array.h>
#include <AK/StringView.h>

namespace UsersSettings {

static constexpr u32 min_normal_uid = 100;
static constexpr Array account_type_names = { "Standard"sv, "Administrator"sv };
static constexpr StringView wheel_group_name = "wheel"sv;
static constexpr Array default_user_groups = { "users"sv, "window"sv, "audio"sv, "lookup"sv, "phys"sv };
static constexpr Array well_known_home_dirs = { "Desktop"sv, "Documents"sv, "Downloads"sv, "Music"sv, "Pictures"sv, "Videos"sv };

}
