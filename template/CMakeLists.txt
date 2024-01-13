
include_directories( ${CMAKE_SOURCE_DIR}/libs    )


########### next target ###############

set(kmuddytemplateplugin_SRCS plugin_template.cpp )

kcoreaddons_add_plugin(kmuddytemplateplugin
SOURCES
${kmuddytemplateplugin_SRCS}
INSTALL_NAMESPACE
    "kmuddy"
)

target_link_libraries(kmuddytemplateplugin KF5::CoreAddons kmuddycore )


