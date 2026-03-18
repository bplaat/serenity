@GUI::Frame {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [10]
        spacing: 5
    }

    @GUI::GroupBox {
        title: "Clock format"
        preferred_height: "fit"
        layout: @GUI::VerticalBoxLayout {
            margins: [16, 8, 8]
            spacing: 8
        }

        @GUI::Label {
            text: "Set the format used by the taskbar clock."
            text_alignment: "TopLeft"
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::VerticalBoxLayout {
                spacing: 4
            }

            @GUI::RadioButton {
                name: "hhmm_radio"
                text: "Hours and minutes"
            }

            @GUI::RadioButton {
                name: "hhmmss_radio"
                text: "Hours, minutes and seconds"
            }

            @GUI::HorizontalSeparator {}

            @GUI::RadioButton {
                name: "custom_radio"
                text: "Custom format:"
            }

            @GUI::Widget {
                preferred_height: "fit"
                layout: @GUI::HorizontalBoxLayout {
                    spacing: 4
                }

                @GUI::Label {
                    fixed_width: 18
                }

                @GUI::TextBox {
                    name: "custom_format_input"
                }
            }
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 4
            }

            @GUI::Label {
                text: "Preview:"
                text_alignment: "CenterLeft"
                preferred_width: 120
            }

            @GUI::Label {
                frame_style: "SunkenContainer"
                name: "clock_preview"
                text: "15:30:45"
            }
        }
    }
}
