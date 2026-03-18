/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <LibCore/DateTime.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/Dialog.h>

namespace GUI {

class TimePicker : public Dialog {
    C_OBJECT(TimePicker)

public:
    virtual ~TimePicker() override = default;

    static Optional<Core::DateTime> show(Window* parent_window, String title, Core::DateTime focused_time = Core::DateTime::now(), bool use_24h = true);

private:
    explicit TimePicker(Window* parent_window, String const& title, Core::DateTime focused_time = Core::DateTime::now(), bool use_24h = true);

    Core::DateTime m_selected_time;
    RefPtr<SpinBox> m_hour_spinbox;
    RefPtr<SpinBox> m_minute_spinbox;
    RefPtr<SpinBox> m_second_spinbox;
    RefPtr<ComboBox> m_ampm_combobox;
};

}
