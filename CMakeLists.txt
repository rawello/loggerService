cmake_minimum_required(VERSION 3.15)
project(logger_service)

include(${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.h)

find_package(Drogon REQUIRED)

add_executable(logger_service ${SOURCES} ${HEADERS})

target_include_directories(logger_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(COPY ${CMAKE_SOURCE_DIR}/config.json DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/generators/)

target_link_libraries(logger_service PRIVATE
    Drogon::Drogon     
    pthread
)