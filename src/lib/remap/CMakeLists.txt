add_library(libremap STATIC Remap.cpp)
target_include_directories(libremap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
