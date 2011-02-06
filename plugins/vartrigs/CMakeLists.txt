
include_directories( ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )


########### next target ###############

set(kmuddyvartrigplugin_PART_SRCS 
   vartrigplugin.cpp 
   cvartrigger.cpp 
   cvartriggerlist.cpp
   cvartriggereditor.cpp
   )

kde4_add_plugin(kmuddyvartrigplugin ${kmuddyvartrigplugin_PART_SRCS})

target_link_libraries(kmuddyvartrigplugin ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} kmuddycore )

install(TARGETS kmuddyvartrigplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddyvartrigplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )



