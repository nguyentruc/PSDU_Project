cmake_minimum_required (VERSION 2.6)

# Define project name
project (aggregator)

set(SUPPRESSED_WARNING "-Wno-conversion-null -Wno-write-strings")
set(LIBRARIES boost_system boost_thread boost_log boost_log_setup boost_locale boost_program_options pthread)

# Compiling flags
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SUPPRESSED_WARNING}")

# Add include directories
include_directories(${CMAKE_SOURCE_DIR})

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#===== aggregator =====#
set ( aggregator_SRCS
    main.cpp
    Aggregator.cpp
    jsoncpp.cpp
    PowerHandler.cpp
    Sim900Handler.cpp
    Client.cpp
    utils.cpp
    sms_pdu.cpp
)
add_executable(aggregator ${aggregator_SRCS})
target_link_libraries(aggregator ${LIBRARIES})
