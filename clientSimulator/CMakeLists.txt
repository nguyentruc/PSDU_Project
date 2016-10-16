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

set (	test_story0017_1_SRCS
	test_story0017_1.cpp
)
add_executable(test_story0017_1 ${test_story0017_1_SRCS})
target_link_libraries(test_story0017_1 story_static)

set (	test_story0017_2_SRCS
	test_story0017_2.cpp
)
add_executable(test_story0017_2 ${test_story0017_2_SRCS})
target_link_libraries(test_story0017_2 story_static)


# set (	test_story0016_1_SRCS
# 	test_story0016_1.cpp
# )
# add_executable(test_story0016_1 ${test_story0016_1_SRCS})
# target_link_libraries(test_story0016_1 story_static)

# set (	test_story0016_2_SRCS
# 	test_story0016_2.cpp
# )
# add_executable(test_story0016_2 ${test_story0016_2_SRCS})
# target_link_libraries(test_story0016_2 story_static)

# set (	test_story0016_3_SRCS
# 	test_story0016_3.cpp
# )
# add_executable(test_story0016_3 ${test_story0016_3_SRCS})
# target_link_libraries(test_story0016_3 story_static)

# set (	test_story0014_1_SRCS
# 	test_story0014_1.cpp
# )
# add_executable(test_story0014_1 ${test_story0014_1_SRCS})
# target_link_libraries(test_story0014_1 story_static)

# set (	test_story0014_2_SRCS
# 	test_story0014_2.cpp
# )
# add_executable(test_story0014_2 ${test_story0014_2_SRCS})
# target_link_libraries(test_story0014_2 story_static)

# set (	test_story0014_3_SRCS
# 	test_story0014_3.cpp
# )
# add_executable(test_story0014_3 ${test_story0014_3_SRCS})
# target_link_libraries(test_story0014_3 story_static)

# set (	test_story0014_4_SRCS
# 	test_story0014_4.cpp
# )
# add_executable(test_story0014_4 ${test_story0014_4_SRCS})
# target_link_libraries(test_story0014_4 story_static)

# set (	test_story0014_5_SRCS
# 	test_story0014_5.cpp
# )
# add_executable(test_story0014_5 ${test_story0014_5_SRCS})
# target_link_libraries(test_story0014_5 story_static)

# set (	test_story0014_6_SRCS
# 	test_story0014_6.cpp
# )
# add_executable(test_story0014_6 ${test_story0014_6_SRCS})
# target_link_libraries(test_story0014_6 story_static)

# set (	test_story0014_7_SRCS
# 	test_story0014_7.cpp
# )
# add_executable(test_story0014_7 ${test_story0014_7_SRCS})
# target_link_libraries(test_story0014_7 story_static)

# set (	test_story0008_1_SRCS
# 	test_story0008_1.cpp
# )
# add_executable(test_story0008_1 ${test_story0008_1_SRCS})
# target_link_libraries(test_story0008_1 story_static)

# set (	test_story0008_2_SRCS
# 	test_story0008_2.cpp
# )
# add_executable(test_story0008_2 ${test_story0008_2_SRCS})
# target_link_libraries(test_story0008_2 story_static)

# set (	test_story0008_3_SRCS
# 	test_story0008_3.cpp
# )
# add_executable(test_story0008_3 ${test_story0008_3_SRCS})
# target_link_libraries(test_story0008_3 story_static)

# set (	test_story0008_4_SRCS
# 	test_story0008_4.cpp
# )
# add_executable(test_story0008_4 ${test_story0008_4_SRCS})
# target_link_libraries(test_story0008_4 story_static)

# set (	test_storyUnknown_SRCS
# 	test_storyUnknown.cpp
# )
# add_executable(test_storyUnknown ${test_storyUnknown_SRCS})
# target_link_libraries(test_storyUnknown story_static)
