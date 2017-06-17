project(kmuddy)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} )

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(KF5 CONFIG REQUIRED KCoreAddons I18n KIOCore Phonon)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE))
include(KDECMakeSettings)
include(FeatureSummary)
include(KDEFrameworkCompilerSettings)

macro_optional_find_package(MXP)
if (MXP_FOUND)
  message(STATUS "Found the MXP library at " ${MXP_LIBRARIES})
  set(HAVE_MXP TRUE)
else (MXP_FOUND)
  message(STATUS "MXP library was NOT found.")
endif (MXP_FOUND)

configure_file(config-mxp.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-mxp.h )

#set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(${QT_DEFINITIONS})

#include(ConvenienceLibs.cmake)

# for config-*.h files
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory( libs )
add_subdirectory( kmuddy )
add_subdirectory( plugins )
add_subdirectory( scripting )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install ( FILES muds.xml DESTINATION ${DATA_INSTALL_DIR}/kmuddy )



