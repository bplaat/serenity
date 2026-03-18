@GUI::TimePickerDialogWidget {
    shrink_to_fit: true
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [8]
        spacing: 8
    }

    @GUI::Widget {
        preferred_height: "fit"
        layout: @GUI::HorizontalBoxLayout {
            spacing: 4
        }

        @GUI::SpinBox {
            name: "hour_spinbox"
            min: 0
            max: 23
            fixed_size: [55, 22]
        }

        @GUI::Label {
            text: ":"
            text_alignment: "Center"
            fixed_width: 8
        }

        @GUI::SpinBox {
            name: "minute_spinbox"
            min: 0
            max: 59
            fixed_size: [55, 22]
        }

        @GUI::Label {
            text: ":"
            text_alignment: "Center"
            fixed_width: 8
        }

        @GUI::SpinBox {
            name: "second_spinbox"
            min: 0
            max: 59
            fixed_size: [55, 22]
        }

        @GUI::ComboBox {
            name: "ampm_combobox"
            fixed_height: 22
        }
    }

    @GUI::Widget {
        fixed_height: 22
        fill_with_background_color: true
        layout: @GUI::HorizontalBoxLayout {}

        @GUI::Layout::Spacer {}

        @GUI::Button {
            name: "cancel_button"
            text: "Cancel"
            fixed_size: [80, 22]
        }

        @GUI::Button {
            name: "ok_button"
            text: "OK"
            fixed_size: [80, 22]
            default: true
        }
    }
}
