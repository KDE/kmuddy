
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kmuddyvarviewerplugin_PART_SRCS
  varviewerplugin.cpp
  dlgvarviewer.cpp
)

kde4_add_plugin(kmuddyvarviewerplugin ${kmuddyvarviewerplugin_PART_SRCS})

target_link_libraries(kmuddyvarviewerplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} kmuddycore )

install(TARGETS kmuddyvarviewerplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddyvarviewerplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )


