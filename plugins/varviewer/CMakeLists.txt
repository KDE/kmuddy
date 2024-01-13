
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR}    )


########### next target ###############

set(kmuddyvarviewerplugin_SRCS
  varviewerplugin.cpp
  dlgvarviewer.cpp
)

kcoreaddons_add_plugin(kmuddyvarviewerplugin
SOURCES
${kmuddyvarviewerplugin_SRCS}
INSTALL_NAMESPACE
    "kmuddy"
)

target_link_libraries(kmuddyvarviewerplugin KF5::CoreAddons kmuddycore )

