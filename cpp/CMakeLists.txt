cmake_minimum_required(VERSION 3.18)
set(PROJECT_NAME "parser_102")

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
set(BOOST_ROOT "dev/task24_tcp_server/cpp")
set(BoostNO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADING ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUINTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
    message.pb.h
    message.pb.cc
    main.cpp
    DelimitedMessagesStreamParser.h
    DelimitedMessagesStreamParser.cpp
    test_api.h test_api.cpp
)

target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

enable_testing()
add_executable(
    message.pb.h
    message.pb.cc
    test.cpp
    test_api.h
    test_api.cpp
    DelimitedMessagesStreamParser.h
    DelimitedMessagesStreamParser.cpp
)

include(GoogleTest)
