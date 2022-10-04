
include_directories( ${CMAKE_SOURCE_DIR}/libs    )


########### next target ###############

set(kmuddystringsplugin_PART_SRCS stringsplugin.cpp )

add_library(kmuddystringsplugin MODULE ${kmuddystringsplugin_PART_SRCS})

target_link_libraries(kmuddystringsplugin  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddystringsplugin DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES  kmuddystringsplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )


