/*
 * Copyright (c) 2022-2022, Olivier De Cannière <olivier.decanniere96@gmail.com>
 * Copyright (c) 2022, Tobias Christiansen <tobyase@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "CalendarSettingsWidget.h"
#include <LibConfig/Client.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/ItemListModel.h>

namespace CalendarSettings {

void CalendarSettingsWidget::apply_settings()
{
    Config::write_string("Calendar"sv, "View"sv, "DefaultView"sv, m_default_view_combobox->text());
}

void CalendarSettingsWidget::reset_default_values()
{
    m_default_view_combobox->set_text("Month");
}

ErrorOr<void> CalendarSettingsWidget::initialize()
{
    m_default_view_combobox = *find_descendant_of_type_named<GUI::ComboBox>("default_view");
    m_default_view_combobox->set_text(Config::read_string("Calendar"sv, "View"sv, "DefaultView"sv, "Month"sv));
    m_default_view_combobox->set_only_allow_values_from_model(true);
    m_default_view_combobox->set_model(GUI::ItemListModel<StringView, Array<StringView, 2>>::create(m_view_modes));
    m_default_view_combobox->on_change = [&](auto, auto) {
        set_modified(true);
    };
    return {};
}

}
