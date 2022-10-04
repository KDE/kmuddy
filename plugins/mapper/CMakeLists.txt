
include_directories( ${CMAKE_SOURCE_DIR}/libs   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )


########### next target ###############

set(kmuddymapper_PART_SRCS 
   kmuddy_mapper.cpp 
   cmaptoolbase.cpp 
   cmaplevel.cpp 
   cmapview.cpp 
   cmapwidget.cpp 
   cmapdata.cpp 
   cmapelement.cpp 
   cmappath.cpp 
   cmaproom.cpp 
   cmapzone.cpp 
   cmaptext.cpp 
   cmapelementutil.cpp
   cmapviewstatusbar.cpp 
   cmapcmdgroup.cpp 
   cmapcmdelementcreate.cpp 
   cmapcmdelementproperties.cpp 
   cmapcmdelementdelete.cpp 
   cmapcmdtogglepathtwoway.cpp 
   cmapcmdmovemap.cpp 
   cmapcmdmoveplayer.cpp 
   cmapcmdleveldelete.cpp 
   cmapcmdlevelcreate.cpp 
   cmappropertiespanebase.cpp 
   cmappluginbase.cpp 
   cmapfilter.cpp 
   cmapfilefilterbase.cpp 
   cmapcommand.cpp 
   cmapclipboard.cpp 
   cmapzonemanager.cpp 
   kmemconfig.cpp 
   cmapmanager.cpp
   dialogs/dlgmapmovement.cpp
   dialogs/dlgmaptextproperties.cpp
   dialogs/dlgmaproomproperties.cpp
   dialogs/dlgmappathproperties.cpp
   dialogs/dlgspeedwalkprogress.cpp
   dialogs/dlgmapcolor.cpp
   dialogs/dlgmapspeedwalk.cpp
   dialogs/dlgmapdirections.cpp 
   filefilters/cmapfilefilterxml.cpp

   plugins/standard/cmapcmdmoveelements.cpp 
   plugins/standard/cmapcmdnotes.cpp 
   plugins/standard/cmappluginstandard.cpp
   plugins/standard/tools/cmaptoolselect.cpp
   plugins/standard/tools/cmaptoolroom.cpp
   plugins/standard/tools/cmaptoolpath.cpp
   plugins/standard/tools/cmaptooltext.cpp
   plugins/standard/tools/cmaptooleraser.cpp
   plugins/standard/propertyPanes/cmapnotespane.cpp
   )

ki18n_wrap_ui (kmuddymapper_PART_SRCS
   dialogs/dlgmapcolorbase.ui
   dialogs/dlgmapmovementbase.ui
   dialogs/dlgmapspeedwalkbase.ui
   dialogs/dlgmaptextpropertiesbase.ui
   dialogs/dlgmaproompropertiesbase.ui
   dialogs/dlgmappathpropertiesbase.ui
   dialogs/dlgspeedwalkprogressbase.ui
   dialogs/dlgmapdirectionsbase.ui
   )

add_library(kmuddymapper MODULE ${kmuddymapper_PART_SRCS})

target_link_libraries(kmuddymapper  KF5::KDELibs4Support KF5::Parts KF5::WidgetsAddons KF5::Service KF5::Archive kmuddycore )

install(TARGETS kmuddymapper DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES kmuddymapper.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )
install( FILES kmuddymapperpart.rc DESTINATION ${KDE_INSTALL_DATADIR}/kmuddy )
install( FILES kmuddymapperplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR} )
install( FILES kmud_grid.png kmud_create.png kmud_follow.png DESTINATION ${KDE_INSTALL_DATADIR}/kmuddy/toolbar )
install( FILES plugins/standard/tools/kmud_select.png plugins/standard/tools/kmud_room.png plugins/standard/tools/kmud_text.png plugins/standard/tools/kmud_path.png plugins/standard/tools/kmud_eraser.png DESTINATION ${KDE_INSTALL_DATADIR}/kmuddy/toolbar )

