get_filename_component(DIRNAME ${CMAKE_CURRENT_LIST_DIR} NAME)

project(day-${DIRNAME} LANGUAGES CXX)
configure_file(input.txt input.txt COPYONLY)
add_executable(${PROJECT_NAME} main.cpp)

find_package(Boost REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)