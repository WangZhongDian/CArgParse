cmake_minimum_required(VERSION 3.28)

project(CArgParse)

if(MSVC)
    add_compile_options(/utf-8)
endif(MSVC)


option(SHARED_BUILD "Build shared library" OFF)
option(TEST "Build tests" ON)
option(EXAMPLE "Build examples" ON)

set(CMAKE_C_STANDARD 11)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

aux_source_directory(src SRC)

if(SHARED_BUILD)
    message(STATUS "Building shared library")
    add_library(${PROJECT_NAME} SHARED ${SRC})
else()
    message(STATUS "Building static library")
    add_library(${PROJECT_NAME} ${SRC})
endif(SHARED_BUILD)

if(TEST)
    add_subdirectory(tests)
endif(TEST)

if(EXAMPLE)
    add_subdirectory(examples)
endif(EXAMPLE)



# install
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES include/CArgParse.h DESTINATION include)
