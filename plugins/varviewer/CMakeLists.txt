
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR}    )


########### next target ###############

set(kmuddyvarviewerplugin_PART_SRCS
  varviewerplugin.cpp
  dlgvarviewer.cpp
)

add_library(kmuddyvarviewerplugin MODULE ${kmuddyvarviewerplugin_PART_SRCS})

target_link_libraries(kmuddyvarviewerplugin  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddyvarviewerplugin DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES  kmuddyvarviewerplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )


