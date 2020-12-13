project(advent-of-code-lib)

add_library(${PROJECT_NAME} utils.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
