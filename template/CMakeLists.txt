
include_directories( ${CMAKE_SOURCE_DIR}/lib ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kmuddyplugintemplate_PART_SRCS plugin_template.cpp )

kde4_add_plugin(kmuddyplugintemplate ${kmuddyplugintemplate_PART_SRCS})

target_link_libraries(kmuddyplugintemplate  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} kmuddycore )

install(TARGETS kmuddyplugintemplate DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddyplugintemplate.desktop DESTINATION ${SERVICES_INSTALL_DIR} )



