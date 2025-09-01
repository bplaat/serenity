serenity_component(
    UsersSettings
    RECOMMENDED
    TARGETS UsersSettings
)

compile_gml(UsersTab.gml UsersTabGML.cpp)
compile_gml(UserAddDialog.gml UserAddDialogGML.cpp)

set(SOURCES
    main.cpp
    UsersTab.cpp
    UsersTabGML.cpp
    UserAddDialogGML.cpp
)

# FIXME: Add icon
serenity_app(UsersSettings ICON filetype-executable)
target_link_libraries(UsersSettings PRIVATE LibCore LibGfx LibGUI LibMain)
