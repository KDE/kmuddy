
include_directories( ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

add_definitions (-DQT3_SUPPORT)
add_definitions (-DQT3_SUPPORT_WARNINGS)

########### next target ###############

set(kmuddymapper_standard_PART_SRCS 
   cmapcmdmoveelements.cpp 
   cmapcmdnotes.cpp 
   cmappluginstandard.cpp
   tools/cmaptoolselect.cpp
   tools/cmaptoolroom.cpp
   tools/cmaptoolpath.cpp
   tools/cmaptooltext.cpp
   tools/cmaptooleraser.cpp
   propertyPanes/cmapnotespane.cpp )

kde4_add_plugin(kmuddymapper_standard WITH_PREFIX ${kmuddymapper_standard_PART_SRCS})

target_link_libraries(kmuddymapper_standard  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDE3SUPPORT_LIBS} kmuddymapper )

install(TARGETS kmuddymapper_standard DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kmuddymapper_standard.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES kmuddymapper_standard.rc DESTINATION ${DATA_INSTALL_DIR}/kmuddy )
install( FILES tools/kmud_select.png tools/kmud_room.png tools/kmud_text.png tools/kmud_path.png tools/kmud_eraser.png DESTINATION ${DATA_INSTALL_DIR}/kmuddy/toolbar )

