cmake_minimum_required(VERSION 3.20)

project(BoDealer LANGUAGES CXX)

include(CTest)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BO_RUNTIME_DIR "${CMAKE_BINARY_DIR}/bin")

file(GLOB DDS_SOURCES CONFIGURE_DEPENDS "dds-develop/src/*.cpp")
add_library(dds STATIC ${DDS_SOURCES})
target_include_directories(dds PUBLIC dds-develop/src)
target_compile_definitions(dds PRIVATE DDS_THREADS_STL)
target_link_libraries(dds PUBLIC Threads::Threads)

add_executable(oscar
    sln/Oscar/OsBook.cpp
    sln/Oscar/Oscar.cpp
    sln/Oscar/OsHelpers.cpp
    sln/Oscar/OsServer.cpp
    sln/Oscar/TaskRegistry.cpp
)
target_include_directories(oscar PRIVATE sln/Oscar walrus)
target_link_libraries(oscar PRIVATE Threads::Threads)

file(GLOB WALRUS_SOURCES CONFIGURE_DEPENDS "walrus/*.cpp")
add_executable(walrus ${WALRUS_SOURCES} dds-develop/examples/hands.cpp sln/Oscar/OsBook.cpp)
target_include_directories(walrus PRIVATE walrus dds-develop/src dds-develop/examples sln/Oscar)
target_link_libraries(walrus PRIVATE dds Threads::Threads)

if(WIN32)
    set_target_properties(walrus PROPERTIES OUTPUT_NAME "Walrus")
    set_target_properties(oscar PROPERTIES OUTPUT_NAME "Oscar")
else()
    find_package(Curses REQUIRED)
    target_include_directories(walrus PRIVATE ${CURSES_INCLUDE_DIRS})
    target_link_libraries(walrus PRIVATE ${CURSES_LIBRARIES})
endif()

foreach(target IN ITEMS walrus oscar)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${BO_RUNTIME_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BO_RUNTIME_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BO_RUNTIME_DIR}"
    )
    if(MSVC)
        target_compile_definitions(${target} PRIVATE NOMINMAX)
    endif()
endforeach()

add_dependencies(walrus oscar)

if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    foreach(case IN ITEMS missing-config invalid-config successful-calculation statistical-calculation)
        add_test(
            NAME regression.${case}
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/regression.py
                    --walrus $<TARGET_FILE:walrus> --case ${case}
        )
    endforeach()
endif()
