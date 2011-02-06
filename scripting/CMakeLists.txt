
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  kmuddy-usock-client.pl kmuddy-usock-test.pl kmuddyvars.h locktest.c pre.c README testserver.cpp tmpserver.pl vartest.c DESTINATION ${DATA_INSTALL_DIR}/kmuddy )



#original Makefile.am contents follow:

#INCLUDES = 
#METASOURCES = AUTO
#pkgdata_SCRIPTS =  kmuddy-usock-client.pl kmuddy-usock-test.pl kmuddyvars.h locktest.c pre.c README testserver.cpp tmpserver.pl vartest.c
