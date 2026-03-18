serenity_component(
    TimeServer
    RECOMMENDED
    TARGETS TimeServer
)

set(SOURCES
    main.cpp
    TimeServer.cpp
)

serenity_bin(TimeServer)
target_link_libraries(TimeServer PRIVATE LibConfig LibCore LibMain)
