cmake_minimum_required(VERSION 2.8)

project(ff_vacation)
add_definitions(-std=c++11)
#add_definitions(-g)
add_definitions(-O2)
add_definitions(-DUSING_FF_NONBLOCKING_QUEUE)
add_definitions(-DNDEBUG)

include_directories(${PROJECT_SOURCE_DIR}/../ff/include)
message(${PROJECT_SOURCE_DIR}/../ff/include)

set(LIBS_LINK_LIBS ff pthread)
link_directories(${PROJECT_SOURCE_DIR}/../bin)



#add_definitions(-std=c++11)
#add_definitions(-g)

include_directories(${PROJECT_SOURCE_DIR}/common)

set(CMN_SRC common/random.cpp)

set(VACATION_SRC ${CMN_SRC}
vacation/args.cpp
vacation/client.cpp
vacation/customer.cpp
vacation/main.cpp
vacation/manager.cpp
vacation/record.cpp)


ADD_EXECUTABLE(vacation ${VACATION_SRC})
target_link_libraries(vacation boost_program_options ${LIBS_LINK_LIBS})
