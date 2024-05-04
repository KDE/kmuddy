
add_subdirectory( dialogs )

include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR} )
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
   cbutton.cpp
   cbuttonlist.cpp
   cbuttoneditor.cpp
   cscriptapi.cpp
   kmuddy.cpp 
   main.cpp 
)

add_executable(kmuddy ${kmuddy_SRCS})

set (KMUDDY_LIBRARIES kmuddycore dialogs KF6::I18n KF6::TextWidgets KF6::XmlGui KF6::IconThemes KF6::Notifications z )
if (MXP_FOUND)
  set (KMUDDY_LIBRARIES ${KMUDDY_LIBRARIES} ${MXP_LIBRARIES})
endif (MXP_FOUND)

target_link_libraries(kmuddy ${KMUDDY_LIBRARIES} )

install(TARGETS kmuddy DESTINATION ${KDE_INSTALL_BINDIR})

# install the icons
ecm_install_icons(ICONS
    16-apps-kmuddy.png
    32-apps-kmuddy.png
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)
ecm_install_icons(ICONS
    16-apps-kmuddy.png
    32-apps-kmuddy.png
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME locolor
)

install (FILES org.kde.kmuddy.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install (FILES kmuddy.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})

