
include_directories( ${CMAKE_SOURCE_DIR}/libs   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )


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

add_library(kmuddyscriptingplugin MODULE ${kmuddyscriptingplugin_PART_SRCS})

target_link_libraries(kmuddyscriptingplugin KF5::KDELibs4Support KF5::Parts kmuddycore )

install(TARGETS kmuddyscriptingplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddyscriptingplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )



