cmake_minimum_required(VERSION 3.5)

project(UnitTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ./bin)
add_executable(UnitTest main.cpp)

find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBMONGOCXX_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBBSONCXX_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

target_link_libraries(${PROJECT_NAME} PRIVATE mongo::bsoncxx_static)
target_link_libraries(${PROJECT_NAME} PRIVATE mongo::mongocxx_static)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/startup.sh DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/unittest.sh DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/end.sh DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/UserFile DESTINATION ${EXECUTABLE_OUTPUT_PATH})
