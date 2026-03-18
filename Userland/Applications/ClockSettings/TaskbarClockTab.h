/*
 * Copyright (c) 2022, cflip <cflip@cflip.net>
 * Copyright (c) 2026, Bastiaan van der Plaat <bastiaan.v.d.plaat@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefPtr.h>
#include <LibCore/Timer.h>
#include <LibGUI/Label.h>
#include <LibGUI/RadioButton.h>
#include <LibGUI/SettingsWindow.h>
#include <LibGUI/TextBox.h>

class TaskbarClockTab final : public GUI::SettingsWindow::Tab {
    C_OBJECT_ABSTRACT(TaskbarClockTab)

public:
    static ErrorOr<NonnullRefPtr<TaskbarClockTab>> try_create();

private:
    TaskbarClockTab() = default;
    ErrorOr<void> setup();

    virtual void apply_settings() override;
    virtual void reset_default_values() override;

    void update_clock_preview();

    RefPtr<GUI::RadioButton> m_hhmm_radio;
    RefPtr<GUI::RadioButton> m_hhmmss_radio;
    RefPtr<GUI::TextBox> m_custom_format_input;
    RefPtr<GUI::Label> m_clock_preview;

    RefPtr<Core::Timer> m_clock_preview_update_timer;

    ByteString m_time_format;
};
