/*
 * Copyright (c) 2024, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "FileSystemSearchModel.h"
#include <AK/LexicalPath.h>
#include <LibGUI/FileIconProvider.h>

namespace FileManager {

void FileSystemSearchModel::set_paths(Vector<ByteString> paths)
{
    m_paths = move(paths);
    invalidate();
}

GUI::Variant FileSystemSearchModel::data(GUI::ModelIndex const& index, GUI::ModelRole role) const
{
    if (!index.is_valid())
        return {};

    auto const& path = m_paths[index.row()];

    if (role == GUI::ModelRole::Icon && index.column() == Column::Icon)
        return GUI::FileIconProvider::icon_for_path(path);

    if (role != GUI::ModelRole::Display)
        return {};

    if (index.column() == Column::Name)
        return LexicalPath(path).basename();

    if (index.column() == Column::SymlinkTarget)
        return path;

    return {};
}

}
