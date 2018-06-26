FIND_PACKAGE(Boost COMPONENTS unit_test_framework REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ../src ${CMAKE_INCLUDE_PATH} )
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK -DTESTS_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

FILE(GLOB unit_tests "*_test.cc")
FOREACH(unit_test ${unit_tests})
  # strip the .cc suffix
  GET_FILENAME_COMPONENT(unit_test_bin ${unit_test} NAME_WE)
  ADD_EXECUTABLE(${unit_test_bin} ${unit_test})
  TARGET_LINK_LIBRARIES (${unit_test_bin} yui-qt-pkg ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
  ADD_TEST(NAME ${unit_test_bin} COMMAND ${unit_test_bin})
ENDFOREACH(unit_test)
