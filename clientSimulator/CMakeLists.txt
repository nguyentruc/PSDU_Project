cmake_minimum_required (VERSION 2.6)

# Define project name
project (clientSimulator)

set(PSDU_ROOT "/home/trucndt/workspace/PSDU_Project")

set(SUPPRESSED_WARNING "-Wno-conversion-null -Wno-write-strings")
set(LIBRARIES boost_system boost_program_options bluetooth)

# Compiling flags
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SUPPRESSED_WARNING}")

# Add include directories
#include_directories(${CMAKE_SOURCE_DIR})
include_directories(${PSDU_ROOT}/aggregator)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#===== aggregator =====#
set ( story_SRCS
    ${PSDU_ROOT}/aggregator/jsoncpp.cpp
    ${PSDU_ROOT}/aggregator/utils.cpp
    ClientSimulator.cpp
)
add_library(story_static STATIC ${story_SRCS})
target_link_libraries(story_static ${LIBRARIES})

set (	test_story0014_SRCS
	test_story0014.cpp
)
add_executable(test_story0014 ${test_story0014_SRCS})
target_link_libraries(test_story0014 story_static)

set (	test_storyUnknown_SRCS
	test_storyUnknown.cpp
)
add_executable(test_storyUnknown ${test_storyUnknown_SRCS})
target_link_libraries(test_storyUnknown story_static)
