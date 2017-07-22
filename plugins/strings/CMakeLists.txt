
include_directories( ${CMAKE_SOURCE_DIR}/libs    )


########### next target ###############

set(kmuddystringsplugin_PART_SRCS stringsplugin.cpp )

add_library(kmuddystringsplugin MODULE ${kmuddystringsplugin_PART_SRCS})

target_link_libraries(kmuddystringsplugin  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddystringsplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddystringsplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} )


