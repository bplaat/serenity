/*
 * Copyright (c) 2023, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ELFWidget.h"
#include "AK/Demangle.h"
#include "AK/LexicalPath.h"

namespace ELFExplorer {

ErrorOr<NonnullRefPtr<ELFWidget>> ELFWidget::create()
{
    auto widget = TRY(try_create());
    TRY(widget->setup());
    return widget;
}

ErrorOr<void> ELFWidget::setup()
{
    // Get all widgets from GML
    m_icon_image = *find_descendant_of_type_named<GUI::ImageWidget>("icon_image");
    m_path_label = *find_descendant_of_type_named<GUI::Label>("path_label");
    m_type_label = *find_descendant_of_type_named<GUI::Label>("type_label");
    m_exports_list = *find_descendant_of_type_named<GUI::ListView>("exports_tab");
    m_imports_list = *find_descendant_of_type_named<GUI::ListView>("imports_tab");
    m_strings_list = *find_descendant_of_type_named<GUI::ListView>("strings_tab");

    return {};
}

ErrorOr<void> ELFWidget::open_file(ByteString const& path)
{
    // Open and parse ELF file
    auto basename = LexicalPath::basename(path);
    auto file = TRY(Core::MappedFile::map(path));
    auto elf_image_data = file->bytes();
    ELF::Image elf_image(elf_image_data);
    if (!elf_image.is_valid()) {
        return Error::from_string_view("File is not a valid ELF object"sv);
    }

    auto const& header = *reinterpret_cast<Elf_Ehdr const*>(elf_image_data.data());

    // Update UI
    m_path_label->set_text(MUST(String::from_byte_string(basename)));
    m_type_label->set_text(MUST(String::formatted("ELF64 - {} - {}",
        ELF::Image::object_file_type_to_string(header.e_type).value_or("?"sv),
        ELF::Image::object_machine_type_to_string(header.e_machine).value_or("?"sv))));

    // Exports
    m_exports.clear();
    elf_image.for_each_symbol([this](auto const& sym) {
        if (sym.type() == STB_LOCAL) {
            m_exports.append(MUST(String::from_byte_string(AK::demangle(sym.name()))));
        }
    });
    m_exports_list->set_model(*GUI::ItemListModel<String>::create(m_exports));

    // Imports
    m_imports.clear();
    elf_image.for_each_symbol([this](auto const& sym) {
        if (sym.type() == STB_GLOBAL || sym.type() == STB_WEAK) {
            m_imports.append(MUST(String::from_byte_string(AK::demangle(sym.name()))));
        }
    });
    m_imports_list->set_model(*GUI::ItemListModel<String>::create(m_imports));

    // Strings
    m_strings.clear();
    m_strings_list->set_model(*GUI::ItemListModel<String>::create(m_strings));

    return {};
}

}
