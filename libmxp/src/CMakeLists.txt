include_directories (${CMAKE_SOURCE_DIR})

set (mxp_SRCS
    celementmanager.cpp
    cmxpcolors.cpp
    cmxpprocessor.cpp
    cresulthandler.cpp
    stringops.cpp
    centitymanager.cpp
    cmxpparser.cpp
    cmxpstate.cpp
    libmxp.cpp
)

add_library(mxp SHARED ${mxp_SRCS})

set_target_properties(mxp PROPERTIES VERSION 0.0.5 SOVERSION 0)
install(TARGETS mxp DESTINATION lib${LIB_SUFFIX} )

install (FILES libmxp.h DESTINATION include/libmxp)

