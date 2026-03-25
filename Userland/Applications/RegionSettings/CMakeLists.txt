serenity_component(
    RegionSettings
    RECOMMENDED
    TARGETS RegionSettings
)

stringify_gml(RegionSettingsWidget.gml RegionSettingsWidgetGML.h region_settings_widget_gml)

set(SOURCES
    main.cpp
    RegionSettingsWidget.cpp
)

set(GENERATED_SOURCES
    RegionSettingsWidgetGML.h
)

serenity_app(RegionSettings ICON app-hello-world)
target_link_libraries(RegionSettings PRIVATE LibConfig LibCore LibGfx LibGUI LibMain LibLocale)
