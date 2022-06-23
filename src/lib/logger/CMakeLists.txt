add_library(liblogger STATIC Logger.cpp)
target_include_directories(liblogger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
