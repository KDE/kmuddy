
include_directories( ${CMAKE_SOURCE_DIR}/libs   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )


########### next target ###############

set(kmuddyscriptingplugin_SRCS 
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


kcoreaddons_add_plugin(kmuddyscriptingplugin
SOURCES
${kmuddyscriptingplugin_SRCS}
INSTALL_NAMESPACE
    "kmuddy"
)

target_link_libraries(kmuddyscriptingplugin KF6::CoreAddons kmuddycore )

