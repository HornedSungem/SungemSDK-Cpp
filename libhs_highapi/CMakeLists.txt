cmake_minimum_required(VERSION 2.8.3)
project(hs_highapi)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

#----------------------------------------

include_directories(./include)
include_directories(./src)
include_directories(../SungemSDK/include)

#----------------------------------------

set(LIBHS_LIB ../SungemSDK/lib)

if(APPLE)
    set(LIBHS_LIB ../SungemSDK/lib/macos)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LIBHS_LIB ../SungemSDK/lib/linux/${CMAKE_SYSTEM_PROCESSOR})
else()
    message(FATAL_ERROR "Unsupported platforms ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

link_directories(${LIBHS_LIB})

#----------------------------------------

if(UNIX OR APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-operator-names -Wformat -Wformat-security -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
execute_process(COMMAND bash -c "lscpu | grep -qi flags | grep -qi flags | grep -qi f16c" RESULT_VARIABLE SUPPORT_F16C)
if (SUPPORT_F16C EQUAL 0)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mf16c")
  add_definitions(-DSUPPORT_F16C)
endif()

execute_process( COMMAND bash -c "lscpu | grep -qi flags | grep -qi flags | grep -qi sse4_1" RESULT_VARIABLE SUPPORT_SSE41)
if (SUPPORT_SSE41 EQUAL 0)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
endif()

#----------------------------------------

file(GLOB SRC "src/*.cpp")
add_library(${PROJECT_NAME} SHARED ${SRC})
target_link_libraries(${PROJECT_NAME} -lhs -lopencv_core -lopencv_highgui -lopencv_imgproc)
