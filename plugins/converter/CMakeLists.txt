
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR}    )


########### next target ###############

set(kmuddyconverterplugin_SRCS
    converterplugin.cpp
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
    converterdialog.cpp
    )


kcoreaddons_add_plugin(kmuddyconverterplugin
SOURCES
${kmuddyconverterplugin_SRCS}
INSTALL_NAMESPACE
    "kmuddy-plugins"
)

target_link_libraries(kmuddyconverterplugin KF6::CoreAddons kmuddycore )

