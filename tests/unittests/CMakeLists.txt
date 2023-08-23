cmake_minimum_required(VERSION 3.13)
find_package(GTest REQUIRED)
add_executable(constructor_unittest constructor.cc forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h )
add_executable(forthtestsuit1_unittest testsuit1.cc  forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)

set(UNITTEST_TARGETS
forthtestsuit1_unittest
constructor_unittest 
)

add_custom_command(
        TARGET forthtestsuit1_unittest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../external/test-suite/src
                ${CMAKE_CURRENT_BINARY_DIR}/suit1
)
add_custom_command(
        TARGET forthtestsuit1_unittest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../testoutput
                ${CMAKE_CURRENT_BINARY_DIR}/testresult
)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_EXTENSIONS OFF)

include(GoogleTest)
foreach(T ${UNITTEST_TARGETS})
  target_link_libraries(${T} PRIVATE GTest::GTest)
  gtest_discover_tests(${T})
endforeach()
