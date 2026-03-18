/*
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefPtr.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibCore/DateTime.h>
#include <LibCore/Timer.h>
#include <LibGUI/Button.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/Label.h>
#include <LibGUI/RadioButton.h>
#include <LibGUI/SettingsWindow.h>
#include <LibGUI/TextBox.h>

class DateTimeTab final : public GUI::SettingsWindow::Tab {
    C_OBJECT_ABSTRACT(DateTimeTab)

public:
    static ErrorOr<NonnullRefPtr<DateTimeTab>> try_create();

private:
    DateTimeTab() = default;
    ErrorOr<void> setup();
    virtual void apply_settings() override;
    virtual void reset_default_values() override;
    void load_current_time();
    void update_manual_controls_enabled();
    void update_ntp_server_controls();

    Core::DateTime m_loaded_time;

    RefPtr<GUI::Label> m_current_time_label;
    RefPtr<Core::Timer> m_clock_update_timer;

    RefPtr<GUI::Button> m_date_button;
    RefPtr<GUI::Button> m_time_button;

    RefPtr<GUI::CheckBox> m_auto_sync_checkbox;
    RefPtr<GUI::Widget> m_ntp_controls;
    RefPtr<GUI::ComboBox> m_ntp_server_combobox;
    RefPtr<GUI::Widget> m_custom_ntp_server_row;
    RefPtr<GUI::TextBox> m_custom_ntp_server_input;
    RefPtr<GUI::RadioButton> m_use_24hour_radio;
    Vector<String> m_ntp_server_labels;
};
