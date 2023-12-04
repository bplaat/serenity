serenity_component(
    ELFExplorer
    RECOMMENDED
    TARGETS ELFExplorer
)

compile_gml(ELFWidget.gml ELFWidgetGML.cpp)

set(SOURCES
    main.cpp
    ELFWidgetGML.cpp
    ELFWidget.cpp
)

serenity_app(ELFExplorer ICON filetype-executable) # FIXME: Create ELFExplorer icon
target_link_libraries(ELFExplorer PRIVATE LibCore LibELF LibGfx LibGUI LibFileSystemAccessClient LibMain)
