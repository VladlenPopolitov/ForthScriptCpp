cmake_minimum_required(VERSION 3.15)
find_package(GTest REQUIRED)
add_executable(test_constructor constructor.cc forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h )
add_executable(test_forthtestsuit1 testsuit1.cc  forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)
#add_executable(test_parameters testsuite2.cc  forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)
#add_executable(test_word_to test_word_to.cc  forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)
#add_executable(test_exceptions test_exceptions.cc  forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)
#add_executable(test_strings test_strings.cc  forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)
add_executable(test_wordlist test_wordlist.cc  forthtestlib.cc forthtestlib.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h 
words-check.frt )


set(UNITTEST_TARGETS
#test_parameters
test_forthtestsuit1
test_constructor 
test_wordlist
#test_word_to
#test_exceptions
#test_strings
)
add_custom_command(
        TARGET test_forthtestsuit1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../external/test-suite/src
                $<TARGET_FILE_DIR:test_forthtestsuit1>/
)
add_custom_command(
        TARGET test_forthtestsuit1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/include.fth
                $<TARGET_FILE_DIR:test_forthtestsuit1>/include.fth
)
add_custom_command(
        TARGET test_forthtestsuit1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/include_all.fth
                $<TARGET_FILE_DIR:test_forthtestsuit1>/include_all.fth
)
add_custom_command(
        TARGET test_forthtestsuit1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/include_fp.fth
                $<TARGET_FILE_DIR:test_forthtestsuit1>/include_fp.fth
)
add_custom_command(
        TARGET test_wordlist POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/words-check.frt
                $<TARGET_FILE_DIR:test_forthtestsuit1>/words-check.frt
)
add_custom_command(
        TARGET test_forthtestsuit1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../testoutput
                $<TARGET_FILE_DIR:test_forthtestsuit1>/testresult
)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_EXTENSIONS OFF)

include(GoogleTest)
foreach(T ${UNITTEST_TARGETS})
  target_link_libraries(${T} PRIVATE GTest::GTest)
  gtest_discover_tests(${T})
endforeach()

file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/test_forthtestsuit1.vcxproj.user" 
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
