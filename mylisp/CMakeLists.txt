cmake_minimum_required(VERSION 3.1)
project(MyLisp VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)

set (MyLispSrc main.cpp mylisp.cpp)
set (MyLispHdr mylisp.h)


#add_definitions(-DGTEST_CREATE_SHARED_LIBRARY=1)
#add_definitions(-DGTEST_LINKED_AS_SHARED_LIBRARY=1)
#add_definitions(-DBUILD_SHARED_LIBS=1)
add_subdirectory(gtest)
include(GoogleTest)
#find_package(GTest)
enable_testing()
message(${gtest_SOURCE_DIR})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(MyLisp ${MyLispSrc} ${MyLispHdr})
target_link_libraries(MyLisp gtest)
