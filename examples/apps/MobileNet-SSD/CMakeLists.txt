cmake_minimum_required(VERSION 2.8.3)
project(MobileNet-SSD)

# ----------------------------------------

set(LIBHS ../../../SungemSDK)
set(LIBHS_HIGHAPI ../../../libhs_highapi)

if(APPLE)
    set(LIBHS_LIB ${LIBHS}/lib/macos)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LIBHS_LIB ${LIBHS}/lib/linux/${CMAKE_SYSTEM_PROCESSOR})
else()
    message(FATAL_ERROR "Unsupported platforms ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

# ----------------------------------------

include_directories(${LIBHS_HIGHAPI}/include)

link_directories(${LIBHS_LIB})
link_directories(${LIBHS_HIGHAPI}/lib)

# ----------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# ----------------------------------------

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} -lhs -lhs_highapi -lopencv_core -lopencv_highgui -lopencv_imgproc)
