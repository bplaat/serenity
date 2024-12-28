/*
 * Copyright (c) 2023, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibELF/Image.h>
#include <LibGUI/ImageWidget.h>
#include <LibGUI/ItemListModel.h>
#include <LibGUI/Label.h>
#include <LibGUI/ListView.h>
#include <LibGUI/Widget.h>

namespace ELFExplorer {

class ELFWidget final : public GUI::Widget {
    C_OBJECT_ABSTRACT(ELFWidget)

public:
    static ErrorOr<NonnullRefPtr<ELFWidget>> create();

    ErrorOr<void> open_file(ByteString const& path);

private:
    ELFWidget() = default;

    static ErrorOr<NonnullRefPtr<ELFWidget>> try_create();

    ErrorOr<void> setup();

    RefPtr<GUI::ImageWidget> m_icon_image;
    RefPtr<GUI::Label> m_path_label;
    RefPtr<GUI::Label> m_type_label;
    RefPtr<GUI::ListView> m_exports_list;
    Vector<String> m_exports;
    RefPtr<GUI::ListView> m_imports_list;
    Vector<String> m_imports;
    RefPtr<GUI::ListView> m_strings_list;
    Vector<String> m_strings;
};

}
