add_executable(ForthRobots ForthRobots.cpp )
add_library(ForthRobotsLib display.cpp intrins.cpp motion.cpp screena.cpp crobots.h ../../ForthScriptCpp.h ../../ForthScriptCppFile.h ../../ForthScriptCppMemory.h)

set_property(TARGET ForthRobots PROPERTY CXX_STANDARD 23)
set_property(TARGET ForthRobots PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ForthRobots PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET ForthRobotsLib PROPERTY CXX_STANDARD 23)
set_property(TARGET ForthRobotsLib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ForthRobotsLib PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(ForthRobots  ForthRobotsLib)
target_include_directories(ForthRobots PUBLIC ../..)  
