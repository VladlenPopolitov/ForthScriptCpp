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

file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/forthtestsuit1_unittest.vcxproj.user" 
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
