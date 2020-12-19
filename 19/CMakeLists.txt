get_filename_component(DIRNAME ${CMAKE_CURRENT_LIST_DIR} NAME)

project(day-${DIRNAME} LANGUAGES CXX)
configure_file(input.txt input.txt COPYONLY)
add_executable(${PROJECT_NAME} main.cpp gen1.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE advent-of-code-lib)
