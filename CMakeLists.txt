cmake_minimum_required(VERSION 3.1)

project(grabber CXX)

set(CMAKE_BUILD_TYPE Release) # Release

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-std=c++11 -pedantic -Wall -Wextra)

file(GLOB_RECURSE SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} -lcurl -lz)

if(APPLE)
    add_definitions(${PROJECT_NAME} -stdlib=libc++)
else()
    target_link_libraries(${PROJECT_NAME} -pthread)
endif()
