cmake_minimum_required(VERSION 3.10)
project(vWallopBot)

set(CMAKE_CXX_STANDARD 11)

add_executable(vWallopBot main.cpp)

set(Boost_USE_MULTITHREADED ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(vWallopBot /usr/lib/libTgBot.so ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})
