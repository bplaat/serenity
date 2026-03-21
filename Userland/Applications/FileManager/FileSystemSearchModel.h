/*
 * Copyright (c) 2024, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteString.h>
#include <AK/Vector.h>
#include <LibGUI/AbstractFileSystemModel.h>

namespace FileManager {

class FileSystemSearchModel final : public GUI::AbstractFileSystemModel {
public:
    static NonnullRefPtr<FileSystemSearchModel> create()
    {
        return adopt_ref(*new FileSystemSearchModel());
    }

    void set_paths(Vector<ByteString> paths);

    ByteString path_for_row(int row) const { return m_paths[row]; }

    virtual int row_count(GUI::ModelIndex const& = GUI::ModelIndex()) const override { return m_paths.size(); }
    virtual GUI::Variant data(GUI::ModelIndex const&, GUI::ModelRole) const override;

private:
    FileSystemSearchModel() = default;
    Vector<ByteString> m_paths;
};

}
