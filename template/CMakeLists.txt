
include_directories( ${CMAKE_SOURCE_DIR}/lib    )


########### next target ###############

set(kmuddyplugintemplate_PART_SRCS plugin_template.cpp )

add_library(kmuddyplugintemplate MODULE ${kmuddyplugintemplate_PART_SRCS})

target_link_libraries(kmuddyplugintemplate  KF5::KDELibs4Support KF5::Parts  kmuddycore )

install(TARGETS kmuddyplugintemplate DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES  kmuddyplugintemplate.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )



