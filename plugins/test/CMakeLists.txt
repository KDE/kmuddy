
include_directories( ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

########### next target ###############

set(kmuddytestplugin_PART_SRCS testplugin.cpp )

kde4_add_plugin(kmuddytestplugin ${kmuddytestplugin_PART_SRCS})

target_link_libraries(kmuddytestplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} kmuddycore )

install(TARGETS kmuddytestplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddytestplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

