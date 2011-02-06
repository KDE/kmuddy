
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

add_definitions (-DQT3_SUPPORT)
# add_definitions (-DQT3_SUPPORT_WARNINGS)

########### next target ###############

set(kmuddymapper_speedwalk_PART_SRCS 
   cmappluginspeedwalk.cpp 
   cmapcmdspeedwalkadd.cpp 
   cmapcmdspeedwalkremove.cpp 
   dlgmapspeedwalkcontainter.cpp 
   dlgmapspeedwalklist.cpp )

kde4_add_ui3_files(kmuddymapper_speedwalk_PART_SRCS dlgmapspeedwalklistbase.ui )

kde4_add_plugin(kmuddymapper_speedwalk WITH_PREFIX ${kmuddymapper_speedwalk_PART_SRCS})

target_link_libraries(kmuddymapper_speedwalk  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDE3SUPPORT_LIBS} kmuddymapper )

install(TARGETS kmuddymapper_speedwalk DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddymapper_speedwalk.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES  kmuddymapper_speedwalk.rc DESTINATION ${DATA_INSTALL_DIR}/kmuddy )

