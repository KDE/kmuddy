
include_directories( ${CMAKE_SOURCE_DIR}/lib    )


########### next target ###############

set(kmuddyplugintemplate_PART_SRCS plugin_template.cpp )

add_library(kmuddyplugintemplate MODULE ${kmuddyplugintemplate_PART_SRCS})

target_link_libraries(kmuddyplugintemplate  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddyplugintemplate DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kmuddyplugintemplate.desktop DESTINATION ${SERVICES_INSTALL_DIR} )



