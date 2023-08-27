add_executable(EmbededScript EmbededScript.cpp ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)

set_property(TARGET EmbededScript PROPERTY CXX_STANDARD 20)
set_property(TARGET EmbededScript PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET EmbededScript PROPERTY CXX_EXTENSIONS OFF)

