cmake_minimum_required(VERSION 3.16)

project(kmuddy)

find_package(ECM REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${ECM_MODULE_PATH})

set(QT_MAJOR_VERSION "6")

include(KDEInstallDirs6)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMInstallIcons)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Qml Multimedia)
find_package(KF6 REQUIRED COMPONENTS Config CoreAddons WidgetsAddons I18n KIO KCMUtils TextWidgets XmlGui Service IconThemes Archive Notifications)

find_package(MXP)
if (MXP_FOUND)
  message(STATUS "Found the MXP library at " ${MXP_LIBRARIES})
  set(HAVE_MXP TRUE)
else (MXP_FOUND)
  message(STATUS "MXP library was NOT found.")
endif (MXP_FOUND)

configure_file(config-mxp.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-mxp.h )

#set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(${QT_DEFINITIONS})

# for config-*.h files
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory( libs )
add_subdirectory( kmuddy )
add_subdirectory( plugins )
add_subdirectory( scripting )



########### install files ###############

ki18n_install(po)

