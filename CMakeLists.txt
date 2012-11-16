cmake_minimum_required (VERSION 2.8)
project(BOOST_IRC_SERVER)

include_directories(${BOOST_IRC_SERVER_SOURCE_DIR}/src)
FILE(GLOB SOURCES ${BOOST_IRC_SERVER_SOURCE_DIR}/src/*.cpp)

include(FindDoxygen)

set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.48.0 COMPONENTS regex system date_time)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(server_irc ${SOURCES})
  target_link_libraries(server_irc ${Boost_LIBRARIES})
else()
  message(FATAL "Boost not found!")
endif()

# Documentation
if(DOXYGEN_FOUND)
  find_program(GRAPHVIZ
    dot
    )
  if("${GRAPHVIZ}" STREQUAL "GRAPHVIZ-NOTFOUND")
    message(STATUS "GraphViz not found!")
    set(HAVE_DOT NO)
  else()
    message(STATUS "Found GraphViz: ${GRAPHVIZ}")
    set(HAVE_DOT YES)
  endif()
  configure_file(
    ${BOOST_IRC_SERVER_SOURCE_DIR}/Doxyfile.in 
    Doxyfile
    )

  set(DOXYGEN_INPUT Doxyfile)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT})
else()
  message(STATUS "WARNING: Doxygen not found! You will not be able to generate the documentation.")
endif()
