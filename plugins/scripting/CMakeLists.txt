
include_directories( ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )


########### next target ###############

set(kmuddyscriptingplugin_PART_SRCS 
   scriptingplugin.cpp 
   cscript.cpp 
   cscriptlist.cpp
   cscripteditor.cpp
   cnotifymanager.cpp 
   crunninglist.cpp 
   crunningscript.cpp 
   cunixsocket.cpp 
   dlgrunninglist.cpp 
   )

kde4_add_plugin(kmuddyscriptingplugin ${kmuddyscriptingplugin_PART_SRCS})

target_link_libraries(kmuddyscriptingplugin ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} kmuddycore )

install(TARGETS kmuddyscriptingplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddyscriptingplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )



