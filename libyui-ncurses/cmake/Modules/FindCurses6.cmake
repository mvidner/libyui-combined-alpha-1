# - Try to find curses6
# Once done this will define
#  CURSES_FOUND - System has Curses6
#  CURSES6_INCLUDE_DIRS - The Curses6 include directories
#  CURSES6_LIBRARIES - The libraries needed to use Curses6
#  CURSES6_DEFINITIONS - Compiler switches required for using Curses6

FOREACH(path ${CMAKE_PREFIX_PATH})
  FIND_LIBRARY(CURSES6_NCURSESW_LIBRARY NAMES ncursesw PATHS ${path}/ncurses6 ${path} NO_DEFAULT_PATH)
  FIND_LIBRARY(CURSES6_PANELW_LIBRARY NAMES panelw PATHS ${path}/ncurses6 ${path} NO_DEFAULT_PATH)
  FIND_LIBRARY(CURSES6_TINFO_LIBRARY NAMES tinfo PATHS ${path}/ncurses6 ${LIB} NO_DEFAULT_PATH)
ENDFOREACH()

SET(CURSES6_LIBRARIES ${CURSES6_NCURSESW_LIBRARY} ${CURSES6_PANELW_LIBRARY} ${CURSES6_TINFO_LIBRARY})
FIND_PATH(CURSES6_INCLUDE_DIR ncurses.h PATH_SUFFIXES ncurses6)

SET (CMAKE_CXXFLAGS "${CURSES6_CFLAGS} ${CMAKE_CXXFLAGS} -DNCURSES_INTERNAL=1")
SET (CMAKE_CFLAGS "${CURSES6_CFLAGS} ${CMAKE_CFLAGS} -DNCURSES_INTERNAL=1")


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Curses6  DEFAULT_MSG
                                  CURSES6_LIBRARIES CURSES6_INCLUDE_DIR)

MARK_AS_ADVANCED(CURSES6_INCLUDE_DIR CURSES6_LIBRARIES)