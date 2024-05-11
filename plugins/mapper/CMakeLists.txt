
include_directories( ${CMAKE_SOURCE_DIR}/libs   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )


########### next target ###############

set(kmuddymapper_SRCS 
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

ki18n_wrap_ui (kmuddymapper_SRCS
   dialogs/dlgmapcolorbase.ui
   dialogs/dlgmapmovementbase.ui
   dialogs/dlgmapspeedwalkbase.ui
   dialogs/dlgmaptextpropertiesbase.ui
   dialogs/dlgmaproompropertiesbase.ui
   dialogs/dlgmappathpropertiesbase.ui
   dialogs/dlgspeedwalkprogressbase.ui
   dialogs/dlgmapdirectionsbase.ui
   )


kcoreaddons_add_plugin(kmuddymapperplugin
SOURCES
${kmuddymapper_SRCS}
INSTALL_NAMESPACE
    "kmuddy-plugins"
)

target_link_libraries(kmuddymapperplugin KF6::CoreAddons KF6::WidgetsAddons KF6::Service KF6::Archive kmuddycore )


########### install files ###############

install( FILES kmuddymapperpart.rc DESTINATION ${KDE_INSTALL_DATADIR}/kmuddy )
install( FILES kmud_grid.png kmud_create.png kmud_follow.png DESTINATION ${KDE_INSTALL_DATADIR}/kmuddy/toolbar )
install( FILES plugins/standard/tools/kmud_select.png plugins/standard/tools/kmud_room.png plugins/standard/tools/kmud_text.png plugins/standard/tools/kmud_path.png plugins/standard/tools/kmud_eraser.png DESTINATION ${KDE_INSTALL_DATADIR}/kmuddy/toolbar )

