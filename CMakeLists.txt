cmake_minimum_required(VERSION 3.16)

project(diplom VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(OpenCV_DIR "C:/openCV/opencv/build") 

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_DIR       ${PROJECT_ROOT_DIR}/src)
set(INCLUDE_DIR      ${PROJECT_ROOT_DIR}/include)
set(RESOURCE_DIR     ${PROJECT_ROOT_DIR}/resources)
set(TEST_DIR         ${PROJECT_ROOT_DIR}/test)

set(SRC ${SOURCE_DIR}/engine.cpp)

include_directories("${INCLUDE_DIR}/SFML/include")
link_directories("${INCLUDE_DIR}/SFML/lib")

if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS ${INCLUDE_DIR}/app.rc)
    add_executable(${PROJECT_NAME} WIN32 ${SRC} ${APP_ICON_RESOURCE_WINDOWS})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC})
else()
    add_executable(${PROJECT_NAME} ${SRC})
endif()


target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
)


#find_package(OpenCV REQUIRED)

#target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

if (EXISTS "${INCLUDE_DIR}/precompiled.h")
    target_precompile_headers(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/precompiled.h)
endif()

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory 
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
)


# Добавляем тесты как подпроект
#add_subdirectory(${TEST_DIR})