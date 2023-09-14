
#include <gtest/gtest.h>
#include <string>

#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"
#include "forthtestlib.h"

std::string ValueTo = " ";auto old= R"( )";


TEST(XcodeToTest,XCdashSIZE)
{
    std::string cmd = ValueTo+ R"(  : stack depth depth 0 do . loop ;  stack HEX )";
    std::string res = "0 ";
    RunAndCompare(cmd, res);
    /*
HEX 
T{      0 XC-SIZE -> 1 }T 
T{     7f XC-SIZE -> 1 }T 
T{     80 XC-SIZE -> 2 }T 
T{    7ff XC-SIZE -> 2 }T 
T{    800 XC-SIZE -> 3 }T 
T{   ffff XC-SIZE -> 3 }T 
T{  10000 XC-SIZE -> 4 }T 
T{ 1fffff XC-SIZE -> 4 }T
    */


    cmd += R"( 0 XC-SIZE stack )";
    res += "1 1 ";
    RunAndCompare(cmd, res);
    cmd += R"(  7f XC-SIZE  stack  )";
    res += "1 1 ";
    RunAndCompare(cmd, res);
    cmd += R"(  80 XC-SIZE  stack  )";
    res += "1 2 ";
    RunAndCompare(cmd, res);
    cmd += R"( 7ff XC-SIZE stack  )";
    res += "1 2 ";
    RunAndCompare(cmd, res);
    cmd += R"( 800 XC-SIZE stack  )";
    res += "1 3 ";
    RunAndCompare(cmd, res);
    cmd += R"( ffff XC-SIZE stack  )";
    res += "1 3 ";
    RunAndCompare(cmd, res);
    cmd += R"( 10000 XC-SIZE stack  )";
    res += "1 4 ";
    RunAndCompare(cmd, res);
    cmd += R"( 1fffff XC-SIZE stack  )";
    res += "1 4 ";
    RunAndCompare(cmd, res);
}



TEST(XcodeToTest,XCdashWIDTH)
{
    std::string cmd = ValueTo+ R"(  : stack depth depth 0 do . loop ;  stack  )";
    std::string res = "0 ";
    RunAndCompare(cmd, res);
    /*
T{ $606D XC-WIDTH -> 2 }T 
T{   $41 XC-WIDTH -> 1 }T 
T{ $2060 XC-WIDTH -> 0 }T
    */


    cmd += R"( $606D XC-WIDTH stack )";
    res += "1 2 ";
    RunAndCompare(cmd, res);
    cmd += R"(  $41 XC-WIDTH stack  )";
    res += "1 1 ";
    RunAndCompare(cmd, res);
    cmd += R"(  $2060 XC-WIDTH stack  )";
    res += "1 0 ";
}


TEST(XcodeToTest,XCStorePlusQuestion)
{
    std::string cmd = ValueTo+ R"(  : stack depth depth 0 do . loop ;  stack  )";
    std::string res = "0 ";
    RunAndCompare(cmd, res);
    /*
    T{ $ffff PAD 4 XC!+? -> PAD 3 + 1 <TRUE> }T
    */

    cmd += R"( $ffff PAD 4 XC!+? PAD 3 + 1 ROT = stack )";
    res += "2 -1 -1 ";
    RunAndCompare(cmd, res);
    
}


TEST(XcodeToTest,XHOLD)
{
    std::string cmd = ValueTo+ R"(  : stack depth depth 0 do . loop ;  stack  )";
    std::string res = "0 ";
    RunAndCompare(cmd, res);
    /*
    T{ 0. <# 123. #s 606D xhold #> s" ?123" compare -> 0 }T
    */

    cmd += R"( HEX 0. <# 123. #s 606D xhold #> s" ?123" compare stack )";
    res += "1 0 ";
    RunAndCompare(cmd, res);
    
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

