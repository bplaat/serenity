@UsersSettings::UsersTab {
    layout: @GUI::VerticalBoxLayout {}
    fill_with_background_color: true

    @GUI::HorizontalSplitter {
        @GUI::Widget {
            preferred_width: 150
            layout: @GUI::VerticalBoxLayout {}

            @GUI::ListView {
                name: "users_list"
                should_hide_unnecessary_scrollbars: true
                alternating_row_colors: false
            }

            @GUI::Widget {
                fixed_height: 24
                layout: @GUI::HorizontalBoxLayout {}

                @GUI::Button {
                    name: "add_button"
                    text: "Add"
                }

                @GUI::Button {
                    name: "delete_button"
                    text: "Delete"
                }
            }
        }
    }
}
