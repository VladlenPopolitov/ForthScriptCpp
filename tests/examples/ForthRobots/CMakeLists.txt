cmake_minimum_required(VERSION 3.13)
find_package(GTest REQUIRED)
add_executable(test_ForthRobots test-ForthRobots.cc  ../../../ForthScriptCpp.h ../../../ForthScriptCppFile.h ../../../ForthScriptCppMemory.h )
target_link_libraries(test_ForthRobots PRIVATE ForthRobotsLib)
target_include_directories(test_ForthRobots PUBLIC ../../../)  
target_include_directories(test_ForthRobots PUBLIC ../../../examples/ForthRobots)  

set(UNITTEST_TARGETS
test_ForthRobots
)

set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_EXTENSIONS OFF)

include(GoogleTest)
foreach(T ${UNITTEST_TARGETS})
  target_link_libraries(${T} PRIVATE GTest::GTest)
  gtest_discover_tests(${T})
endforeach()

file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/test_ForthRobots.vcxproj.user" 
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<Project ToolsVersion=\"Current\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">
    <LocalDebuggerWorkingDirectory>${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)\</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|x64'\">
    <LocalDebuggerWorkingDirectory>${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)\</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">
    <LocalDebuggerWorkingDirectory>${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)\</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|x64'\">
    <LocalDebuggerWorkingDirectory>${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)\</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <PropertyGroup>
    <ShowAllFiles>true</ShowAllFiles>
  </PropertyGroup>
</Project>")
