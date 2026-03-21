/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2024, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGUI/Model.h>

namespace GUI {

class AbstractFileSystemModel : public Model {
public:
    enum Column {
        Icon = 0,
        Name,
        Size,
        User,
        Group,
        Permissions,
        ModificationTime,
        Inode,
        SymlinkTarget,
        __Count,
    };

    virtual int tree_column() const override { return Column::Name; }
    virtual int column_count(ModelIndex const& = ModelIndex()) const override { return Column::__Count; }
    virtual ErrorOr<String> column_name(int) const override;
    virtual bool is_column_sortable(int column_index) const override { return column_index != Column::Icon; }
};

}
