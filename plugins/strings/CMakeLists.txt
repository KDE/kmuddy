
include_directories( ${CMAKE_SOURCE_DIR}/libs    )


########### next target ###############

set(kmuddystringsplugin_SRCS stringsplugin.cpp )

kcoreaddons_add_plugin(kmuddystringsplugin
SOURCES
${kmuddystringsplugin_SRCS}
INSTALL_NAMESPACE
    "kmuddy"
)

target_link_libraries(kmuddystringsplugin KF5::CoreAddons kmuddycore )


