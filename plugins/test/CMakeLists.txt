
include_directories( ${CMAKE_SOURCE_DIR}/libs    )

########### next target ###############

set(kmuddytestplugin_SRCS testplugin.cpp )

kcoreaddons_add_plugin(kmuddytestplugin
SOURCES
${kmuddytestplugin_SRCS}
INSTALL_NAMESPACE
    "kmuddy"
)

target_link_libraries(kmuddytestplugin KF6::CoreAddons kmuddycore )

