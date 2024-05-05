
include_directories( 
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR} )


########### next target ###############

set(kmuddycore_LIB_SRCS 
   calias.cpp 
   caliaseditor.cpp 
   caliaslist.cpp 
   cansiparser.cpp 
   ccmdprocessor.cpp 
   cconsole.cpp 
   cdownloader.cpp 
   cmsp.cpp 
   cinputline.cpp
   cprofilesettings.cpp
   cprofilemanager.cpp
   cstatus.cpp 
   ctextchunk.cpp 
   ctabwidget.cpp
   cprompt.cpp 
   csoundplayer.cpp 
   cvariable.cpp 
   cvariablelist.cpp 
   cactionmanager.cpp 
   cactionbase.cpp 
   cmacromanager.cpp 
   cvalue.cpp 
   coutput.cpp 
   ccmdqueue.cpp 
   ccmdqueues.cpp 
   ccmdparser.cpp 
   arith-exp.cpp 
   flex-out.cpp 
   bison-input.tab.cpp 
   cexpresolver.cpp 
   cmenumanager.cpp 
   cdialoglist.cpp 
   cplugin.cpp 
   cpluginmanager.cpp 
   cglobalsettings.cpp 
   cpattern.cpp 
   clistobject.cpp 
   clist.cpp 
   clistgroup.cpp
   clistmanager.cpp
   clistviewer.cpp
   clisteditor.cpp
   clistgroupeditor.cpp
   cscripteval.cpp
   cscripteditor.cpp )

add_library(kmuddycore SHARED ${kmuddycore_LIB_SRCS})

target_link_libraries(kmuddycore KF6::KCMUtils KF6::CoreAddons KF6::I18n KF6::TextWidgets KF6::XmlGui KF6::KIOCore KF6::Completion Qt6::Qml Qt6::Multimedia)

set_target_properties(kmuddycore PROPERTIES VERSION 3.0.0 SOVERSION 3 )
install(TARGETS kmuddycore DESTINATION ${KDE_INSTALL_LIBDIR} )


########### install files ###############

install( FILES cplugin.h arith-exp.h cactionbase.h cactionmanager.h calias.h caliaslist.h cansiparser.h ccmdparser.h ccmdprocessor.h ccmdqueue.h cconsole.h cdownloader.h cexpresolver.h cinputline.h cmacromanager.h cmsp.h coutput.h cprompt.h csoundplayer.h cstatus.h ctextchunk.h cvalue.h cvariable.h cvariablelist.h cmenumanager.h cdialoglist.h cglobalsettings.h clistobject.h clist.h cpattern.h clistgroup.h clistmanager.h clistviewer.h clisteditor.h clistgroupeditor.h cprofilesettings.h cprofilemanager.h ctabwidget.h DESTINATION ${KDE_INSTALL_INCLUDEDIR}/kmuddy )


