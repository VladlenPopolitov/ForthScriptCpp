
#include <gtest/gtest.h>
#include <string>

#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"
#include "forthtestlib.h"

std::string ValueTo = " ";auto old= R"(
    : value create 1 , , does> dup @ 1 = if cell+ @ else then ; 
    : 2value create 2 , , , does> dup @ 2 = if cell+ 2@ else then ;  
    : fvalue create 3 , F,  does> dup @ 3 = if cell+ F@ else then ;
    : to state @ if 3 2 1 then ' >body state @ if 
    postpone literal postpone dup postpone @ postpone dup postpone literal postpone = 
    postpone if postpone drop postpone cell+ postpone ! postpone else postpone dup postpone literal postpone =
    postpone if postpone drop postpone cell+ postpone 2! postpone else postpone literal postpone =
    postpone if postpone cell+ postpone F! postpone then postpone then postpone then  
    else dup @ dup 1 = if drop cell+ ! else dup 2 = if drop cell+ 2! else 3 = if cell+ F! then then then then ; immediate )";


TEST(WordToTest,To_Double)
{
    std::string cmd = ValueTo+ R"(  : stack depth depth 0 do . loop ;  stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
    TESTING 2VALUE

T{ 1111 2222 2VALUE 2VAL -> }T
T{ 2VAL -> 1111 2222 }T
T{ 3333 4444 TO 2VAL -> }T
T{ 2VAL -> 3333 4444 }T
T{ : TO-2VAL TO 2VAL ; 5555 6666 TO-2VAL -> }T
T{ 2VAL -> 5555 6666 }T
    */


    cmd += R"( 1111 2222 2VALUE 2VAL stack )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(   2VAL  stack  )";
    res += "222221111";
    RunAndCompare(cmd, res);
    cmd += R"(  3333 4444 TO 2VAL  stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( 2VAL stack  )";
    res += "244443333";
    RunAndCompare(cmd, res);
    cmd += R"( : TO-2VAL TO 2VAL ; 5555 6666 TO-2VAL stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( 2VAL stack  )";
    res += "266665555";
    RunAndCompare(cmd, res);
}

TEST(WordToTest,To_CoreExt)
{
    std::string cmd = ValueTo+ R"(  : stack depth depth 0 do . loop ; stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);

    //cmd += R"( : VALUE CREATE  , DOES> @ ;  : to       state @ if postpone ['] postpone value! else ' value! then ; immediate stack  )";
    //cmd += R"( : VALUE CREATE  , DOES> @ ;  : TO ' >BODY STATE @ IF POSTPONE LITERAL POSTPONE ! ELSE ! THEN ; IMMEDIATE  stack  )";
    cmd += R"(
    111 VALUE VAL1 -999 VALUE VAL2 
    stack )";
    res += "0";
    RunAndCompare(cmd, res);
  // : fvalue create 3 , F,  does> dup @ dup 3 = if . cell+ F@ else 300000 + . then ;   
  // : 2value create 2 , , , does> dup @ dup 2 = if . cell+ 2@ else 200000 + . then ;  
  // : to ' >body state @ if postpone literal dup @ 1 if postpone cell+ postpone ! then else 555 . dup @ dup 1 = if 777 . . cell+ ! else dup 2 = if 888 . . cell+ 2! else 999 . 3 = if cell+ F! then then then then ; immediate
    /*
TESTING VALUE TO

T{ 111 VALUE VAL1 -999 VALUE VAL2 -> }T
T{ VAL1 -> 111 }T
T{ VAL2 -> -999 }T
T{ 222 TO VAL1 -> }T
T{ VAL1 -> 222 }T
T{ : VD1 VAL1 ; -> }T
T{ VD1 -> 222 }T
T{ : VD2 TO VAL2 ; -> }T
T{ VAL2 -> -999 }T
T{ -333 VD2 -> }T
T{ VAL2 -> -333 }T
T{ VAL1 -> 222 }T
T{ 123 VALUE VAL3 IMMEDIATE VAL3 -> 123 }T
T{ : VD3 VAL3 LITERAL ; VD3 -> 123 }T
    */
    cmd += R"( VAL1 stack )";
    res += "1111";
    RunAndCompare(cmd, res);
    cmd += R"(   VAL2  stack  )"; 
    res += "1-999";
    RunAndCompare(cmd, res);
    cmd += R"(   222 TO VAL1  stack  )"; 
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( VAL1 stack  )"; 
    res += "1222"; 
    RunAndCompare(cmd, res);
    cmd += R"(  : VD2 TO VAL2 ; stack  )"; 
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( VAL2 stack  )";
    res += "1-999";
    RunAndCompare(cmd, res);
    cmd += R"( -333 VD2 stack  )"; 
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( VAL2 stack  )"; 
    res += "1-333";
    RunAndCompare(cmd, res);
    cmd += R"( VAL1 stack  )";
    res += "1222";
    RunAndCompare(cmd, res);
    cmd += R"( 123 VALUE VAL3 IMMEDIATE VAL3 stack  )";
    res += "1123";
    RunAndCompare(cmd, res);
    cmd += R"( : VD3 VAL3 LITERAL ; VD3  stack  )";
    res += "1123";
    RunAndCompare(cmd, res);
}


TEST(WordToTest,To_Float)
{
    std::string cmd = ValueTo+ R"( : fstack fdepth S>F fdepth 0 do F. loop ;  fstack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
    Testing:
T{ 0e0 FVALUE Tval -> }T 
T{ Tval -> 0e0 R}T 
T{ 1e0 TO Tval -> }T 
T{ Tval -> 1e0 R}T 
: setTval Tval FSWAP TO Tval ; 
T{ 2e0 setTval Tval -> 1e0 2e0 RR}T 
T{ 5e0 TO Tval -> }T 
: [execute] EXECUTE ; IMMEDIATE 
T{ ' Tval ] [execute] [ -> 2e0 R}T
    */


    cmd += R"( 0e0 FVALUE Tval fstack )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(   Tval  fstack  )";
    res += "10";
    RunAndCompare(cmd, res);
    cmd += R"(  1e0 TO Tval  fstack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( Tval fstack  )";
    res += "11";
    RunAndCompare(cmd, res);
    cmd += R"( : setTval Tval FSWAP TO Tval ; 2e0 setTval Tval fstack  )";
    res += "221";
    RunAndCompare(cmd, res);
    cmd += R"( 5e0 TO Tval fstack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( : [execute] EXECUTE ; IMMEDIATE ' Tval ] [execute] [ fstack  )";
    res += "12";
    RunAndCompare(cmd, res);
}



int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

