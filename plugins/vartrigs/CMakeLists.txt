
include_directories( ${CMAKE_SOURCE_DIR}/libs   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )


########### next target ###############

set(kmuddyvartrigplugin_PART_SRCS 
   vartrigplugin.cpp 
   cvartrigger.cpp 
   cvartriggerlist.cpp
   cvartriggereditor.cpp
   )

add_library(kmuddyvartrigplugin MODULE ${kmuddyvartrigplugin_PART_SRCS})

target_link_libraries(kmuddyvartrigplugin KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddyvartrigplugin DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES  kmuddyvartrigplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )



