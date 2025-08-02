cmake_minimum_required(VERSION 3.16)

project(parser_tests LANGUAGES CXX)

set(TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(TEST_SRC 
  "${CMAKE_CURRENT_SOURCE_DIR}/test.cc"
  "${MAIN_SRC_DIR}/mp4parser.cc"
  "${MAIN_SRC_DIR}/mp4parser.cc"
  "${MAIN_SRC_DIR}/source.h"
)

find_package(OpenCV REQUIRED)

if(WIN32)
    add_executable(${PROJECT_NAME} ${TEST_SRC})
    add_test(NAME parser_base_test COMMAND ${PROJECT_NAME})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC})
else()
    add_executable(${PROJECT_NAME} ${SRC})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OpenCV_LIBS}
)

# Precompiled header, если нужен
if (EXISTS "${TEST_INCLUDE_DIR}/precompiled.h")
    target_precompile_headers(${PROJECT_NAME} PRIVATE ${TEST_INCLUDE_DIR}/precompiled.h)
endif()

# Копируем ресурсы
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory 
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
)

enable_testing()
add_test(NAME RunUnitTests COMMAND ${PROJECT_NAME})