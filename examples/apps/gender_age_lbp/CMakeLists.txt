cmake_minimum_required(VERSION 2.8.3)
project(gender_age_lbp)

# ----------------------------------------

set(LIBHS ../../../SungemSDK)

if(APPLE)
    set(LIBHS_LIB ${LIBHS}/lib/macos)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LIBHS_LIB ${LIBHS}/lib/linux/${CMAKE_SYSTEM_PROCESSOR})
else()
    message(FATAL_ERROR "Unsupported platforms ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

# ----------------------------------------

include_directories(include)
include_directories(cpp)
include_directories(${LIBHS}/include)
link_directories(${LIBHS_LIB})


# ----------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# ----------------------------------------
find_package(OpenCV)
add_executable(${PROJECT_NAME} cpp/gender_age_lbp.cpp cpp/fp16.c)
#target_link_libraries(${PROJECT_NAME} -lhs -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect)
target_link_libraries(${PROJECT_NAME} -lhs ${OpenCV_LIBS})
