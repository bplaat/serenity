/*
 * Copyright (c) 2024, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "RegionSettingsWidget.h"
#include <Applications/RegionSettings/RegionSettingsWidgetGML.h>
#include <LibConfig/Client.h>
#include <LibCore/DateTime.h>
#include <LibCore/Timer.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/ItemListModel.h>
#include <LibGUI/Label.h>
#include <LibLocale/Locale.h>

struct LocaleEntry {
    StringView tag;
    StringView display_name;
    StringView date_format;
    StringView time_format_24h;
    StringView time_format_12h;
    bool uses_24h;
    StringView decimal_sep;
    StringView thousands_sep;
    StringView number_example;
    StringView first_day_of_week;
    StringView first_day_of_weekend;
    int weekend_length;
};

static constexpr Array<LocaleEntry, 17> k_locales { {
    { "en-US"sv, "United States"sv, "%m/%d/%Y"sv, "%H:%M"sv, "%I:%M %p"sv, false, "."sv, ","sv, "1,234.56"sv, "Sunday"sv, "Saturday"sv, 2 },
    { "en-GB"sv, "United Kingdom"sv, "%d/%m/%Y"sv, "%H:%M"sv, "%I:%M %p"sv, false, "."sv, ","sv, "1,234.56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "en-AU"sv, "Australia"sv, "%d/%m/%Y"sv, "%H:%M"sv, "%I:%M %p"sv, false, "."sv, ","sv, "1,234.56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "en-CA"sv, "Canada"sv, "%Y-%m-%d"sv, "%H:%M"sv, "%I:%M %p"sv, false, "."sv, ","sv, "1,234.56"sv, "Sunday"sv, "Saturday"sv, 2 },
    { "de-DE"sv, "Germany"sv, "%d.%m.%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, "."sv, "1.234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "fr-FR"sv, "France"sv, "%d/%m/%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, " "sv, "1 234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "it-IT"sv, "Italy"sv, "%d/%m/%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, "."sv, "1.234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "es-ES"sv, "Spain"sv, "%d/%m/%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, "."sv, "1.234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "nl-NL"sv, "Netherlands"sv, "%d-%m-%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, "."sv, "1.234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "sv-SE"sv, "Sweden"sv, "%Y-%m-%d"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, " "sv, "1 234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "nb-NO"sv, "Norway"sv, "%d.%m.%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, " "sv, "1 234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "da-DK"sv, "Denmark"sv, "%d.%m.%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, "."sv, "1.234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "fi-FI"sv, "Finland"sv, "%d.%m.%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, " "sv, "1 234,56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "ja-JP"sv, "Japan"sv, "%Y/%m/%d"sv, "%H:%M"sv, "%H:%M"sv, true, "."sv, ","sv, "1,234.56"sv, "Sunday"sv, "Saturday"sv, 2 },
    { "zh-CN"sv, "China"sv, "%Y/%m/%d"sv, "%H:%M"sv, "%H:%M"sv, true, "."sv, ","sv, "1,234.56"sv, "Monday"sv, "Saturday"sv, 2 },
    { "pt-BR"sv, "Brazil"sv, "%d/%m/%Y"sv, "%H:%M"sv, "%I:%M %p"sv, true, ","sv, "."sv, "1.234,56"sv, "Sunday"sv, "Saturday"sv, 2 },
    { "es-MX"sv, "Mexico"sv, "%d/%m/%Y"sv, "%H:%M"sv, "%I:%M %p"sv, false, "."sv, ","sv, "1,234.56"sv, "Sunday"sv, "Saturday"sv, 2 },
} };

static constexpr Array<StringView, 17> k_display_names {
    "United States"sv, "United Kingdom"sv, "Australia"sv, "Canada"sv,
    "Germany"sv, "France"sv, "Italy"sv, "Spain"sv, "Netherlands"sv,
    "Sweden"sv, "Norway"sv, "Denmark"sv, "Finland"sv,
    "Japan"sv, "China"sv, "Brazil"sv, "Mexico"sv
};

ErrorOr<NonnullRefPtr<RegionSettingsWidget>> RegionSettingsWidget::try_create()
{
    auto widget = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) RegionSettingsWidget()));
    TRY(widget->setup());
    return widget;
}

ErrorOr<void> RegionSettingsWidget::setup()
{
    TRY(load_from_gml(region_settings_widget_gml));

    m_region_combo = *find_descendant_of_type_named<GUI::ComboBox>("region_combo");
    m_date_preview = *find_descendant_of_type_named<GUI::Label>("date_preview");
    m_time_preview = *find_descendant_of_type_named<GUI::Label>("time_preview");
    m_number_preview = *find_descendant_of_type_named<GUI::Label>("number_preview");
    m_week_start_preview = *find_descendant_of_type_named<GUI::Label>("week_start_preview");

    m_region_combo->set_only_allow_values_from_model(true);
    m_region_combo->set_model(GUI::ItemListModel<StringView, Array<StringView, 17>>::create(k_display_names));

    // Find and select the currently configured locale.
    auto region_tag = Config::read_string("Locale"sv, "Locale"sv, "Region"sv, "en-US"sv);
    m_selected_index = 0;
    for (size_t i = 0; i < k_locales.size(); ++i) {
        if (k_locales[i].tag == region_tag) {
            m_selected_index = i;
            break;
        }
    }
    m_region_combo->set_text(k_locales[m_selected_index].display_name);

    m_region_combo->on_change = [&](ByteString const&, GUI::ModelIndex const& index) {
        m_selected_index = static_cast<size_t>(index.row());
        set_modified(true);
        update_preview();
    };

    m_preview_timer = Core::Timer::create_repeating(1000, [&]() {
        update_preview();
    });
    m_preview_timer->start();
    update_preview();

    return {};
}

void RegionSettingsWidget::update_preview()
{
    auto const& entry = k_locales[m_selected_index];
    auto now = Core::DateTime::now();

    m_date_preview->set_text(MUST(now.to_string(entry.date_format)));
    m_time_preview->set_text(MUST(now.to_string(entry.uses_24h ? entry.time_format_24h : entry.time_format_12h)));
    m_number_preview->set_text(String::from_utf8(entry.number_example).release_value_but_fixme_should_propagate_errors());
    m_week_start_preview->set_text(String::from_utf8(entry.first_day_of_week).release_value_but_fixme_should_propagate_errors());
}

void RegionSettingsWidget::apply_settings()
{
    auto const& entry = k_locales[m_selected_index];
    Config::write_string("Locale"sv, "Locale"sv, "Region"sv, entry.tag);
    // Push calendar week settings to the domain LibGUI/Calendar reads from.
    Config::write_string("Calendar"sv, "View"sv, "FirstDayOfWeek"sv, entry.first_day_of_week);
    Config::write_string("Calendar"sv, "View"sv, "FirstDayOfWeekend"sv, entry.first_day_of_weekend);
    Config::write_i32("Calendar"sv, "View"sv, "WeekendLength"sv, entry.weekend_length);
}

void RegionSettingsWidget::reset_default_values()
{
    m_selected_index = 0; // en-US
    m_region_combo->set_text(k_locales[0].display_name);
    update_preview();
}
