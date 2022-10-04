
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

install(TARGETS kmuddyscriptingplugin DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES  kmuddyscriptingplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )



