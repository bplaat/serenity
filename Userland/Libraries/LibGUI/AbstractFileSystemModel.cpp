/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2024, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGUI/AbstractFileSystemModel.h>

namespace GUI {

ErrorOr<String> AbstractFileSystemModel::column_name(int column) const
{
    switch (column) {
    case Column::Icon:
        return String {};
    case Column::Name:
        return "Name"_string;
    case Column::Size:
        return "Size"_string;
    case Column::User:
        return "User"_string;
    case Column::Group:
        return "Group"_string;
    case Column::Permissions:
        return "Mode"_string;
    case Column::ModificationTime:
        return "Modified"_string;
    case Column::Inode:
        return "Inode"_string;
    case Column::SymlinkTarget:
        return "Symlink target"_string;
    }
    VERIFY_NOT_REACHED();
}

}
