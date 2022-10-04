
include_directories( ${CMAKE_SOURCE_DIR}/libs    )

########### next target ###############

set(kmuddytestplugin_PART_SRCS testplugin.cpp )

add_library(kmuddytestplugin MODULE ${kmuddytestplugin_PART_SRCS})

target_link_libraries(kmuddytestplugin  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddytestplugin DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES  kmuddytestplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )

