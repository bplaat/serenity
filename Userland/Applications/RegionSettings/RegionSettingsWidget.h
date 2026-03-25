/*
 * Copyright (c) 2024, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefPtr.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/Label.h>
#include <LibGUI/SettingsWindow.h>

class RegionSettingsWidget final : public GUI::SettingsWindow::Tab {
    C_OBJECT_ABSTRACT(RegionSettingsWidget)

public:
    static ErrorOr<NonnullRefPtr<RegionSettingsWidget>> try_create();

private:
    RegionSettingsWidget() = default;
    ErrorOr<void> setup();

    virtual void apply_settings() override;
    virtual void reset_default_values() override;

    void update_preview();

    RefPtr<GUI::ComboBox> m_region_combo;
    RefPtr<GUI::Label> m_date_preview;
    RefPtr<GUI::Label> m_time_preview;
    RefPtr<GUI::Label> m_number_preview;
    RefPtr<GUI::Label> m_week_start_preview;

    RefPtr<Core::Timer> m_preview_timer;

    size_t m_selected_index { 0 };
};
