
include_directories( ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kmuddystringsplugin_PART_SRCS stringsplugin.cpp )

kde4_add_plugin(kmuddystringsplugin ${kmuddystringsplugin_PART_SRCS})

target_link_libraries(kmuddystringsplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} kmuddycore )

install(TARGETS kmuddystringsplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddystringsplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )


