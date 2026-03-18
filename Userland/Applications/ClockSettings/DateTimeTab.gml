@GUI::Frame {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [10]
        spacing: 5
    }

    @GUI::GroupBox {
        title: "Date and time"
        preferred_height: "fit"
        layout: @GUI::VerticalBoxLayout {
            margins: [16, 8, 8]
            spacing: 8
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 4
            }

            @GUI::Label {
                text: "Current time:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::Label {
                name: "current_time_label"
                text: "0000-00-00 00:00:00"
                text_alignment: "CenterLeft"
            }
        }

        @GUI::HorizontalSeparator {}

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 4
            }

            @GUI::Label {
                text: "Date:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::Button {
                name: "date_button"
                fixed_width: 160
            }

            @GUI::Layout::Spacer {}
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 4
            }

            @GUI::Label {
                text: "Time:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::Button {
                name: "time_button"
                fixed_width: 160
            }

            @GUI::Layout::Spacer {}
        }
    }

    @GUI::GroupBox {
        title: "Automatic time synchronization"
        preferred_height: "fit"
        layout: @GUI::VerticalBoxLayout {
            margins: [16, 8, 8]
            spacing: 8
        }

        @GUI::CheckBox {
            name: "auto_sync_checkbox"
            text: "Automatically sync time with an internet time server"
        }

        @GUI::Widget {
            name: "ntp_controls"
            preferred_height: "fit"
            layout: @GUI::VerticalBoxLayout {
                spacing: 6
            }

            @GUI::Widget {
                preferred_height: "fit"
                layout: @GUI::HorizontalBoxLayout {
                    spacing: 4
                }

                @GUI::Label {
                    text: "Source:"
                    text_alignment: "CenterLeft"
                    fixed_width: 120
                }

                @GUI::ComboBox {
                    name: "ntp_server_combobox"
                    model_only: true
                }
            }

            @GUI::Widget {
                name: "custom_ntp_server_row"
                preferred_height: "fit"
                layout: @GUI::HorizontalBoxLayout {
                    spacing: 4
                }

                @GUI::Label {
                    text: ""
                    fixed_width: 120
                }

                @GUI::TextBox {
                    name: "custom_ntp_server_input"
                    placeholder_text: "time.example.com"
                }
            }
        }
    }

    @GUI::GroupBox {
        title: "Time format"
        preferred_height: "fit"
        layout: @GUI::VerticalBoxLayout {
            margins: [16, 8, 8]
            spacing: 4
        }

        @GUI::RadioButton {
            name: "use_24hour_radio"
            text: "24-hour clock (e.g. 15:30)"
        }

        @GUI::RadioButton {
            name: "use_12hour_radio"
            text: "12-hour clock (e.g. 3:30 PM)"
        }
    }

    @GUI::Layout::Spacer {}
}
