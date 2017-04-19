project(vacationdb_tests VERSION 0.1.0)
link_directories(${PROJECT_BINARY_DIR})

file(GLOB SOURCES_LIBVACATIONDB_TEST "*.cpp")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT ${WIN32})
	add_compile_options(-Wno-ctor-dtor-privacy)
endif()
add_executable(vacationdb_tests ${SOURCES_LIBVACATIONDB_TEST})
target_link_libraries(vacationdb_tests gtest)
target_link_libraries(vacationdb_tests vacationdb)