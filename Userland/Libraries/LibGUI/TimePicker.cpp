/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGUI/Button.h>
#include <LibGUI/ItemListModel.h>
#include <LibGUI/SpinBox.h>
#include <LibGUI/TimePicker.h>
#include <LibGUI/TimePickerDialogWidget.h>

namespace GUI {

TimePicker::TimePicker(Window* parent_window, String const& title, Core::DateTime focused_time, bool use_24h)
    : Dialog(parent_window)
    , m_selected_time(focused_time)
{
    if (parent_window)
        set_icon(parent_window->icon());

    set_resizable(false);
    set_title(title.to_byte_string());

    auto widget = MUST(GUI::TimePickerDialogWidget::try_create());
    set_main_widget(widget.ptr());

    m_hour_spinbox = widget->find_descendant_of_type_named<SpinBox>("hour_spinbox");
    m_ampm_combobox = widget->find_descendant_of_type_named<ComboBox>("ampm_combobox");

    if (use_24h) {
        m_hour_spinbox->set_min(0);
        m_hour_spinbox->set_max(23);
        m_hour_spinbox->set_value(focused_time.hour(), AllowCallback::No);
        m_ampm_combobox->set_visible(false);
    } else {
        m_hour_spinbox->set_min(1);
        m_hour_spinbox->set_max(12);
        int display_hour = focused_time.hour() % 12;
        if (display_hour == 0)
            display_hour = 12;
        m_hour_spinbox->set_value(display_hour, AllowCallback::No);

        m_ampm_combobox->set_fixed_size(60, 22);
        static Vector<String> ampm_labels = { "AM"_string, "PM"_string };
        m_ampm_combobox->set_model(ItemListModel<String>::create(ampm_labels));
        m_ampm_combobox->set_only_allow_values_from_model(true);
        m_ampm_combobox->set_selected_index(focused_time.hour() >= 12 ? 1 : 0, AllowCallback::No);
    }

    m_hour_spinbox->on_change = [&, use_24h](int value) {
        int hour;
        if (use_24h) {
            hour = value;
        } else {
            bool is_pm = m_ampm_combobox->selected_index() == 1;
            hour = value % 12 + (is_pm ? 12 : 0);
        }
        m_selected_time.set_time(m_selected_time.year(), m_selected_time.month(), m_selected_time.day(),
            hour, m_selected_time.minute(), m_selected_time.second());
    };

    if (!use_24h) {
        m_ampm_combobox->on_change = [&](auto, auto const& index) {
            bool is_pm = index.row() == 1;
            int display_hour = m_hour_spinbox->value();
            int hour = display_hour % 12 + (is_pm ? 12 : 0);
            m_selected_time.set_time(m_selected_time.year(), m_selected_time.month(), m_selected_time.day(),
                hour, m_selected_time.minute(), m_selected_time.second());
        };
    }

    m_minute_spinbox = widget->find_descendant_of_type_named<SpinBox>("minute_spinbox");
    m_minute_spinbox->set_value(focused_time.minute(), AllowCallback::No);
    m_minute_spinbox->on_change = [&](int value) {
        m_selected_time.set_time(m_selected_time.year(), m_selected_time.month(), m_selected_time.day(),
            m_selected_time.hour(), value, m_selected_time.second());
    };

    m_second_spinbox = widget->find_descendant_of_type_named<SpinBox>("second_spinbox");
    m_second_spinbox->set_value(focused_time.second(), AllowCallback::No);
    m_second_spinbox->on_change = [&](int value) {
        m_selected_time.set_time(m_selected_time.year(), m_selected_time.month(), m_selected_time.day(),
            m_selected_time.hour(), m_selected_time.minute(), value);
    };

    auto& ok_button = *widget->find_descendant_of_type_named<Button>("ok_button");
    ok_button.on_click = [&](auto) {
        done(ExecResult::OK);
    };

    auto& cancel_button = *widget->find_descendant_of_type_named<Button>("cancel_button");
    cancel_button.on_click = [this](auto) {
        done(ExecResult::Cancel);
    };
}

Optional<Core::DateTime> TimePicker::show(Window* parent_window, String title, Core::DateTime focused_time, bool use_24h)
{
    auto picker = TimePicker::construct(parent_window, title, focused_time, use_24h);
    if (picker->exec() == ExecResult::OK)
        return picker->m_selected_time;
    return {};
}

}
