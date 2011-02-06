
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kmuddyconverterplugin_PART_SRCS
    converterplugin.cpp
    converterdialog.cpp
    cgenericitem.cpp
    cgenericlist.cpp
    csaveablefield.cpp
    csaveablelist.cpp
    caction.cpp
    calias.cpp
    cconnprefs.cpp
    cgauge.cpp
    cscript.cpp
    cshortcut.cpp
    cstatusvar.cpp
    ctimer.cpp
    ctrigger.cpp
    cvalue.cpp
    cvariable.cpp
    cvartrigger.cpp
    )

kde4_add_plugin(kmuddyconverterplugin ${kmuddyconverterplugin_PART_SRCS})

target_link_libraries(kmuddyconverterplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} kmuddycore )

install(TARGETS kmuddyconverterplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddyconverterplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

