project(VacationTime VERSION 0.1.0)
link_directories(${PROJECT_BINARY_DIR})

file(GLOB SOURCES_VACATIONTIME "src/*.cpp")

include_directories(include)
include_directories(../libvacationdb/include)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(VacationTime ${SOURCES_VACATIONTIME})
target_link_libraries(VacationTime vacationdb)