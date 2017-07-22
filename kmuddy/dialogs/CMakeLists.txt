
include_directories( ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/kmuddy ${CMAKE_CURRENT_BINARY_DIR}   )

if (MXP_FOUND)
  include_directories (${MXP_INCLUDE_DIR})
endif (MXP_FOUND)


########### next target ###############

set(dialogs_LIB_SRCS
    dlgimportexportprofile.cpp
    dlggrabkey.cpp
    cdirlist.cpp
    dlgstatistics.cpp
    dlgdumpbuffer.cpp
    dlgtranscript.cpp
    dlgprofilesettings.cpp
    dlgmultiline.cpp
    dlgappsettings.cpp
    dlgeditprofile.cpp
    dlgquickconnect.cpp
    dlgconnect.cpp
    dlgoutputwindow.cpp
    dlgwindows.cpp
    dlgmxpconsole.cpp
    dlgobjects.cpp
    dlgmudlist.cpp )

add_library(dialogs STATIC ${dialogs_LIB_SRCS})

target_link_libraries(dialogs )

