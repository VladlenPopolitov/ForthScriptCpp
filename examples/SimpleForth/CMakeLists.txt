
cmake_minimum_required(VERSION 3.15)
add_executable(SimpleForth SimpleForth.cpp ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)
#generate maximal level of compiler warnings
target_compile_options(SimpleForth PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 >
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic >
)

set_property(TARGET SimpleForth PROPERTY CXX_STANDARD 23)
set_property(TARGET SimpleForth PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET SimpleForth PROPERTY CXX_EXTENSIONS OFF)
