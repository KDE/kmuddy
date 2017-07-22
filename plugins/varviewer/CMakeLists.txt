
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR}    )


########### next target ###############

set(kmuddyvarviewerplugin_PART_SRCS
  varviewerplugin.cpp
  dlgvarviewer.cpp
)

add_library(kmuddyvarviewerplugin MODULE ${kmuddyvarviewerplugin_PART_SRCS})

target_link_libraries(kmuddyvarviewerplugin  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddyvarviewerplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddyvarviewerplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )


