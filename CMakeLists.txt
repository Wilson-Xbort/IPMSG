cmake_minimum_required(VERSION 3.0)
PROJECT(ipmsg)

include (cmake/utils.cmake)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

include_directories(./src)
include_directories(/usr/local/include/yaml-cpp)
link_directories(/usr/local/lib)

set(LIB_SRC
    src/log.cpp	
    src/util.cpp	 
    src/config.cpp
    src/thread.cpp
    src/fiber.cpp
)
add_library(ipmsg SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(ipmsg)

find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)

set(LIB_LIB 
	ipmsg
	pthread
	yaml-cpp
)
message(***, ${YAMLCPP})
message(***, ${PTHREAD})

add_executable(ft_test test/test.cpp)
add_dependencies(ft_test ipmsg)
force_redefine_file_macro_for_sources(ft_test)
target_link_libraries(ft_test ipmsg ${LIB_LIB})

add_executable(test_config test/test_config_complate.cpp)
add_dependencies(test_config ipmsg)
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config ipmsg ${LIB_LIB})

add_executable(test_thread test/test_thread.cpp)
add_dependencies(test_thread ipmsg)
force_redefine_file_macro_for_sources(test_thread)
target_link_libraries(test_thread ipmsg ${LIB_LIB})

add_executable(test_util test/test_util.cpp)
add_dependencies(test_util ipmsg)
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util ipmsg ${LIB_LIB})

add_executable(test_fiber test/test_fiber.cpp)
add_dependencies(test_fiber ipmsg)
force_redefine_file_macro_for_sources(test_fiber)
target_link_libraries(test_fiber ipmsg ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)
    		
