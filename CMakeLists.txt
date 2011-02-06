project(kmuddy)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} )

find_package(KDE4 REQUIRED)

macro_optional_find_package(MXP)
if (MXP_FOUND)
  message(STATUS "Found the MXP library at " ${MXP_LIBRARIES})
  set(HAVE_MXP TRUE)
else (MXP_FOUND)
  message(STATUS "MXP library was NOT found.")
endif (MXP_FOUND)

configure_file(config-mxp.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-mxp.h )

#set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

link_directories(${KDE4_LIB_DIR})

#include(ConvenienceLibs.cmake)

# for config-*.h files
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory( libs )
add_subdirectory( kmuddy )
add_subdirectory( plugins )
add_subdirectory( po )
add_subdirectory( scripting )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install ( FILES muds.xml DESTINATION ${DATA_INSTALL_DIR}/kmuddy )



