@ELFExplorer::ELFWidget {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [4]
    }

    @GUI::Widget {
        fixed_height: 128
        layout: @GUI::HorizontalBoxLayout {
            margins: [4]
            spacing: 8
        }

        @GUI::ImageWidget {
            name: "icon_image"
            fixed_width: 32
            fixed_height: 32
        }

        @GUI::Widget {
            layout: @GUI::VerticalBoxLayout {}

            @GUI::Label {
                name: "path_label"
                text_alignment: "CenterLeft"
                text: "This is the filename."
            }

            @GUI::Label {
                name: "type_label"
                text_alignment: "CenterLeft"
                text: "ELF64 - executable - x86_64"
            }
        }
    }

    @GUI::TabWidget {
        @GUI::Widget {
            title: "ELF Header"
            name: "header_tab"
        }

        @GUI::Widget {
            title: "Section Headers"
            name: "sections_tab"
        }

        @GUI::Widget {
            title: "Program Headers"
            name: "sections_tab"
        }

        @GUI::ListView {
            title: "Exports"
            name: "exports_tab"
        }

        @GUI::ListView {
            title: "Imports"
            name: "imports_tab"
        }

        @GUI::ListView {
            title: "Strings"
            name: "strings_tab"
        }

        @GUI::Widget {
            title: "Hex View"
            name: "hex_view_tab"
        }
    }
}
