
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_CURRENT_BINARY_DIR}    )


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

add_library(kmuddyconverterplugin MODULE ${kmuddyconverterplugin_PART_SRCS})

target_link_libraries(kmuddyconverterplugin  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddyconverterplugin DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES  kmuddyconverterplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )

