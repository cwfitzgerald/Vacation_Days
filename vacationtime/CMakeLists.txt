project(VacationTime VERSION 0.1.0)
link_directories(${PROJECT_BINARY_DIR})

include_directories(include)
include_directories(../libvacationdb/include)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE SOURCES_VacationTime "src/*.cpp")
file(GLOB_RECURSE HEADERS_VacationTime "src/*.hpp" "include/*.hpp")
file(GLOB_RECURSE UI_VacationTime "src/*.ui" "include/*.ui")
find_package(Qt5Widgets)

qt5_wrap_cpp(moc_sources ${HEADERS_VacationTime})
qt5_wrap_ui(uic_sources ${UI_VacationTime})

add_executable(VacationTime ${SOURCES_VacationTime} ${moc_sources} ${uic_sources})
target_link_libraries(VacationTime vacationdb)
target_link_libraries(VacationTime Qt5::Widgets)