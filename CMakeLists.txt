cmake_minimum_required(VERSION 3.16)

set(KMUDDY_VERSION "1.1")

project(kmuddy VERSION ${KMUDDY_VERSION})

#set(QT_MAJOR_VERSION "6")
set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.0.0")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${ECM_MODULE_PATH})
set(CMAKE_CXX_STANDARD 17)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMInstallIcons)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Widgets Qml Multimedia)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS Config CoreAddons WidgetsAddons I18n KIO KCMUtils TextWidgets XmlGui Service IconThemes Archive Codecs Notifications)

find_package(MXP)
if (MXP_FOUND)
  message(STATUS "Found the MXP library at " ${MXP_LIBRARIES})
  set(HAVE_MXP TRUE)
else (MXP_FOUND)
  message(STATUS "MXP library was NOT found.")
endif (MXP_FOUND)

configure_file(config-mxp.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-mxp.h )

#set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# I have no idea what causes these to be defined. But they are breaking things all over the place.
remove_definitions(
  -DQT_NO_KEYWORDS
  -DQT_NO_CAST_FROM_ASCII
)

# for config-*.h files
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory( libs )
add_subdirectory( kmuddy )
add_subdirectory( plugins )
add_subdirectory( scripting )



########### install files ###############

ki18n_install(po)

