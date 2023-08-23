cmake_minimum_required(VERSION 3.13)
find_package(GTest REQUIRED)
add_executable(constructor_unittest constructor.cc)
set(UNITTEST_TARGETS
constructor_unittest
)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_EXTENSIONS OFF)

include(GoogleTest)
foreach(T ${UNITTEST_TARGETS})
  target_link_libraries(${T} PRIVATE GTest::GTest)
  gtest_discover_tests(${T})
endforeach()
