cmake_minimum_required(VERSION 3.11)
project(Boost.Application CXX)
include(conan.cmake)

set(CXX_STANDART 17)

file(GLOB_RECURSE SRC_LIST ./include/**/*.cpp)
file(GLOB_RECURSE HEADER_LIST ./include/**/*.hpp)

file(GLOB_RECURSE TEST_SRC_LIST ./test/*.cpp)
file(GLOB_RECURSE TEST_HEADER_LIST ./test/*.hpp)

file(GLOB_RECURSE APP_SRC_LIST ./test/myapp.cpp)
file(GLOB_RECURSE APP_HEADER_LIST ./test/myapp.hpp)

conan_cmake_run(REQUIRES boost/1.70.0@conan/stable
                BASIC_SETUP
                BUILD missing)


add_library(myapp STATIC ${APP_SRC_LIST} ${APP_HEADER_LIST})


include_directories(include)

enable_testing()
foreach(item ${TEST_SRC_LIST})
    get_filename_component(temp_name ${item} NAME)
    if(NOT (${temp_name} STREQUAL myapp.cpp ))
        add_definitions(-DBOOST_TEST_STATIC_LINK -DBOOST_TEST_MAIN)
	    add_executable(${temp_name}  ${SRC_LIST} ${HEADER_LIST} ${item} ${TEST_HEADER_LIST})
        target_link_libraries(${temp_name} ${CONAN_LIBS} rt myapp)
        add_test(NAME ${temp_name}_test COMMAND ${temp_name})
    endif()
endforeach()
