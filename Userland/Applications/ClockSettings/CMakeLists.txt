serenity_component(
    ClockSettings
    RECOMMENDED
    TARGETS ClockSettings
)

stringify_gml(TaskbarClockTab.gml TaskbarClockTabGML.h taskbar_clock_tab_gml)
stringify_gml(DateTimeTab.gml DateTimeTabGML.h date_time_tab_gml)
stringify_gml(TimeZoneTab.gml TimeZoneTabGML.h time_zone_tab_gml)

set(SOURCES
    main.cpp
    DateTimeTab.cpp
    TaskbarClockTab.cpp
    TimeZoneTab.cpp
)

set(GENERATED_SOURCES
    DateTimeTabGML.h
    TaskbarClockTabGML.h
    TimeZoneTabGML.h
)

serenity_app(ClockSettings ICON app-analog-clock) # FIXME: Create a ClockSettings icon.
target_link_libraries(ClockSettings PRIVATE LibConfig LibCore LibGfx LibGUI LibMain LibLocale)
