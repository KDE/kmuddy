# - Try to find MXP
# Once done this will define
#
#  MXP_FOUND - system has MXP
#  MXP_INCLUDE_DIR - the MXP include directory
#  MXP_LIBRARIES - Link these to use MXP
#  MXP_DEFINITIONS - Compiler switches required for using MXP
# Redistribution and use is allowed according to the terms of the BSD license.

if ( MXP_INCLUDE_DIR AND MXP_LIBRARIES )
   # in cache already
   SET(MXP_FIND_QUIETLY TRUE)
endif ( MXP_INCLUDE_DIR AND MXP_LIBRARIES )

set(MXP_INCLUDE_DIR)
set(MXP_LIBRARIES)

FIND_PATH(MXP_INCLUDE_DIR NAMES libmxp/libmxp.h
)

FIND_LIBRARY(MXP_LIBRARIES NAMES mxp
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MXP DEFAULT_MSG MXP_INCLUDE_DIR MXP_LIBRARIES )

# show the MXP_INCLUDE_DIR and MXP_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(MXP_INCLUDE_DIR MXP_LIBRARIES )

