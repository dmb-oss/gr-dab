INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_DAB grdab)

FIND_PATH(
    DAB_INCLUDE_DIRS
    NAMES grdab/api.h
    HINTS $ENV{DAB_DIR}/include
        ${PC_DAB_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    DAB_LIBRARIES
    NAMES gnuradio-grdab
    HINTS $ENV{DAB_DIR}/lib
        ${PC_DAB_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/dabTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DAB DEFAULT_MSG DAB_LIBRARIES DAB_INCLUDE_DIRS)
MARK_AS_ADVANCED(DAB_LIBRARIES DAB_INCLUDE_DIRS)
