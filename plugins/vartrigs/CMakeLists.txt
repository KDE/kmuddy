
include_directories( ${CMAKE_SOURCE_DIR}/libs   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )


########### next target ###############

set(kmuddyvartrigplugin_SRCS 
   vartrigplugin.cpp 
   cvartrigger.cpp 
   cvartriggerlist.cpp
   cvartriggereditor.cpp
   )

kcoreaddons_add_plugin(kmuddyvartrigplugin
SOURCES
${kmuddyvartrigplugin_SRCS}
INSTALL_NAMESPACE
    "kmuddy"
)

target_link_libraries(kmuddyvartrigplugin KF6::CoreAddons kmuddycore )

