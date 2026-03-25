@CalendarSettings::CalendarSettingsWidget {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [8]
        spacing: 5
    }

    @GUI::GroupBox {
        title: "Default view"
        fixed_height: 72
        layout: @GUI::VerticalBoxLayout {
            margins: [6]
            spacing: 2
        }

        @GUI::Label {
            text: "Show the month or the year view when Calendar is started."
            text_wrapping: "Wrap"
            text_alignment: "CenterLeft"
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 16
            }

            @GUI::Label {
                text: "Default view:"
                text_alignment: "CenterLeft"
                fixed_width: 70
            }

            @GUI::ComboBox {
                name: "default_view"
            }
        }
    }
}
