cmake_minimum_required(VERSION 3.14)

include("beforetarget.cmake" OPTIONAL)

add_library(observable_lib INTERFACE)
target_include_directories(observable_lib INTERFACE include)
target_link_libraries(observable_lib INTERFACE ${all_libraries})
