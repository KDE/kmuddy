
add_subdirectory( dialogs )

include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDES} ${KDE4_PHONON_INCLUDES} ${QT_INCLUDES} )
if (MXP_FOUND)
  include_directories (${MXP_INCLUDE_DIR})
endif (MXP_FOUND)

########### next target ###############

set(kmuddy_SRCS 
   cshortcut.cpp 
   cshortcutlist.cpp 
   cshortcuteditor.cpp 
   ctimerlist.cpp 
   ctimer.cpp 
   ctimereditor.cpp
   cmccp.cpp 
   ctranscript.cpp 
   ctextprocessor.cpp 
   cmultilineinput.cpp 
   cconnection.cpp 
   csession.cpp 
   ctelnet.cpp 
   kmuddy.cpp 
   main.cpp 
   ctrigger.cpp 
   ctriggerlist.cpp
   ctriggereditor.cpp
   cwindowlist.cpp 
   cmxpmanager.cpp 
   cmxpconsole.cpp 
   cstatusvar.cpp 
   cstatusvarlist.cpp 
   cstatusvareditor.cpp 
   cgauge.cpp 
   cgaugelist.cpp 
   cgaugeeditor.cpp 
   cgaugebaritem.cpp 
   cgaugebar.cpp 
   cmultiinputline.cpp 
   macros.cpp 
   csessionmanager.cpp
   cmudlist.cpp
   cbutton.cpp
   cbuttonlist.cpp
   cbuttoneditor.cpp
   cscriptapi.cpp
)

kde4_add_executable(kmuddy ${kmuddy_SRCS})

set (KMUDDY_LIBRARIES kmuddycore dialogs ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_PHONON_LIBS} ${KDE4_KDE3SUPPORT_LIBS} z )

if (MXP_FOUND)
  set (KMUDDY_LIBRARIES ${KMUDDY_LIBRARIES} ${MXP_LIBRARIES})
endif (MXP_FOUND)

target_link_libraries(kmuddy ${KMUDDY_LIBRARIES} )

install(TARGETS kmuddy DESTINATION ${BIN_INSTALL_DIR})

# install the icons
kde4_install_icons( ${ICON_INSTALL_DIR} )

install (FILES kmuddy.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

