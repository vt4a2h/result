cmake_minimum_required(VERSION 3.17)

project(
    result
    VERSION 0.1
    DESCRIPTION "A simple implementation of the Result type"
    LANGUAGES CXX
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CXX_STANDARD 20)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    
    include(CTest)

    if(BUILD_TESTING)
        enable_testing()
        add_subdirectory(test)
    endif()
endif()
