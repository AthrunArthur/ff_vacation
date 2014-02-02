cmake_minimum_required(VERSION 2.8)

project(ff_bench)

add_definitions(-std=c++11)
add_definitions(-g)

include_directories(${PROJECT_SOURCE_DIR}/common)

set(CMN_SRC common/random.cpp)

set(VACATION_SRC ${CMN_SRC}
vacation/args.cpp
vacation/client.cpp
vacation/customer.cpp
vacation/main.cpp
vacation/manager.cpp)


ADD_EXECUTABLE(vacation ${VACATION_SRC})