
add_executable(SimpleForth SimpleForth.cpp ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)

set_property(TARGET SimpleForth PROPERTY CXX_STANDARD 20)
set_property(TARGET SimpleForth PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET SimpleForth PROPERTY CXX_EXTENSIONS OFF)
