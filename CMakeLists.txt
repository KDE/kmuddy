project(kmuddy)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${ECM_MODULE_PATH})

# temporary, cleaning the errors
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMInstallIcons)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Script Multimedia)
find_package(KF5 REQUIRED COMPONENTS Config CoreAddons WidgetsAddons I18n KIO KCMUtils TextWidgets XmlGui Service IconThemes Archive Notifications)

find_package(MXP)
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



########### install files ###############

ki18n_install(po)

