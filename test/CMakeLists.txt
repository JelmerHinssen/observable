cmake_minimum_required(VERSION 3.14)

include("beforetarget.cmake" OPTIONAL)

file(GLOB OBSERVABLE_TEST
    "*.h"
    "*.cpp"
)
add_executable(observable_test ${OBSERVABLE_TEST})
target_link_libraries(observable_test observable_lib gtest_main)
target_include_directories(observable_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src ${CMAKE_CURRENT_SOURCE_DIR}/../src/include/observable)

set_target_properties(observable_test PROPERTIES OUTPUT_NAME observable_test)

add_custom_command(TARGET observable_test POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:observable_test> ${CMAKE_SOURCE_DIR}/bin/)

