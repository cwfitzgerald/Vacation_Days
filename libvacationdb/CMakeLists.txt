project(vacationdb VERSION 0.1.0)
link_directories(${PROJECT_BINARY_DIR})

file(GLOB SOURCES_LIBVACATIONDB "src/*.cpp")

include_directories(include)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Boost_USE_STATIC_LIBS       OFF) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.36.0 COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

find_package(GMP)
include_directories(${GMP_INCLUDE_DIRS})
link_libraries(${GMP_LIBRARIES})

add_library(vacationdb SHARED ${SOURCES_LIBVACATIONDB})

add_subdirectory(tests)