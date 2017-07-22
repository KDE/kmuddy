
include_directories( ${CMAKE_SOURCE_DIR}/libs    )

########### next target ###############

set(kmuddytestplugin_PART_SRCS testplugin.cpp )

add_library(kmuddytestplugin MODULE ${kmuddytestplugin_PART_SRCS})

target_link_libraries(kmuddytestplugin  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddytestplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddytestplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

