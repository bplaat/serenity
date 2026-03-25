@GUI::Frame {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [10]
        spacing: 8
    }

    @GUI::GroupBox {
        title: "Region"
        shrink_to_fit: false
        layout: @GUI::VerticalBoxLayout {
            margins: [16, 8, 8]
            spacing: 8
        }

        @GUI::Label {
            text: "Choose a region to set date, time, and number formatting conventions."
            text_alignment: "TopLeft"
            text_wrapping: "Wrap"
            preferred_height: "fit"
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Region:"
                text_alignment: "CenterLeft"
                fixed_width: 60
            }

            @GUI::ComboBox {
                name: "region_combo"
            }
        }
    }

    @GUI::GroupBox {
        title: "Preview"
        shrink_to_fit: true
        layout: @GUI::VerticalBoxLayout {
            margins: [16, 8, 8]
            spacing: 4
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 4
            }

            @GUI::Label {
                text: "Date:"
                text_alignment: "CenterLeft"
                fixed_width: 120
            }

            @GUI::Label {
                name: "date_preview"
                frame_style: "SunkenContainer"
                text: ""
            }
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 4
            }

            @GUI::Label {
                text: "Time:"
                text_alignment: "CenterLeft"
                fixed_width: 120
            }

            @GUI::Label {
                name: "time_preview"
                frame_style: "SunkenContainer"
                text: ""
            }
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 4
            }

            @GUI::Label {
                text: "Number:"
                text_alignment: "CenterLeft"
                fixed_width: 120
            }

            @GUI::Label {
                name: "number_preview"
                frame_style: "SunkenContainer"
                text: ""
            }
        }

        @GUI::Widget {
            preferred_height: "fit"
            layout: @GUI::HorizontalBoxLayout {
                spacing: 4
            }

            @GUI::Label {
                text: "First day of week:"
                text_alignment: "CenterLeft"
                fixed_width: 120
            }

            @GUI::Label {
                name: "week_start_preview"
                frame_style: "SunkenContainer"
                text: ""
            }
        }
    }
}
