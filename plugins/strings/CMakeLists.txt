
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR}   )


########### next target ###############

set(kmuddystringsplugin_SRCS stringsplugin.cpp )

kcoreaddons_add_plugin(kmuddystringsplugin
SOURCES
${kmuddystringsplugin_SRCS}
INSTALL_NAMESPACE
    "kmuddy-plugins"
)

target_link_libraries(kmuddystringsplugin KF6::CoreAddons kmuddycore )


