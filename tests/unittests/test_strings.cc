
#include <gtest/gtest.h>
#include <string>

#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"
#include "forthtestlib.h"

std::string ValueTo=R"(   
:  S1 S" abcdefghijklmnopqrstuvwxyz" ; 
 :  S2 S" abc"   ; 
 :  S3 S" jklmn" ; 
 :  S4 S" z"     ; 
 :  S5 S" mnoq"  ; 
 :  S6 S" 12345" ; 
 :  S7 S" "      ; 
 :  S8 S" abc  " ;
 :  S9 S"      " ;
 : S10 S"    a " ; ;
 )";



TEST(Strings,Compare)
{
    std::string cmd = ValueTo+ R"( : stack depth  depth 0 do . loop ;  stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
   TESTING COMPARE

T{ S1 S1 COMPARE -> 0 }T
T{ S1 PAD SWAP CMOVE -> }T
T{ S1 PAD OVER COMPARE -> 0 }T
T{ S1 PAD 6 COMPARE -> 1 }T
T{ PAD 10 S1 COMPARE -> -1 }T
T{ S1 PAD 0 COMPARE -> 1 }T
T{ PAD 0 S1 COMPARE -> -1 }T
T{ S1 S6 COMPARE ->  1 }T
T{ S6 S1 COMPARE -> -1 }T
T{ S7 PAD 0 COMPARE -> 0 }T

T{ S1 $" abdde"  COMPARE -> -1 }T
T{ S1 $" abbde"  COMPARE ->  1 }T
T{ S1 $" abcdf"  COMPARE -> -1 }T
T{ S1 $" abcdee" COMPARE ->  1 }T

: S11 S" 0abc" ;
: S12 S" 0aBc" ;

T{ S11 S12  COMPARE -> 1 }T
T{ S12 S11  COMPARE -> -1 }T

    */
    cmd += R"( S1 S1 COMPARE stack )";
    res += "10";
    RunAndCompare(cmd, res);
    cmd += R"(S1 PAD SWAP CMOVE  stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(S1 PAD OVER COMPARE   stack  )";
    res += "10";
    RunAndCompare(cmd, res);
    cmd += R"(S1 PAD 6 COMPARE  stack  )";
    res += "11";
    RunAndCompare(cmd, res);
    cmd += R"( PAD 10 S1 COMPARE  stack  )";
    res += "1-1";
    RunAndCompare(cmd, res);
}



TEST(Strings,Search)
{
    std::string cmd = ValueTo+ R"( : stack depth  depth 0 do . loop ;  stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
  TESTING SEARCH

T{ S1 S2 SEARCH -> S1 TRUE }T
T{ S1 S3 SEARCH -> S1  9 /STRING TRUE }T
T{ S1 S4 SEARCH -> S1 25 /STRING TRUE }T
T{ S1 S5 SEARCH -> S1 FALSE }T
T{ S1 S6 SEARCH -> S1 FALSE }T
T{ S1 S7 SEARCH -> S1 TRUE }T
T{ S7 PAD 0 SEARCH -> S7 TRUE }T

    */
    cmd += R"( S1 S2 SEARCH stack )";
    res += "3______-1";
    RunAndCompare(cmd, res);
    cmd += R"(S1 S3 SEARCH  stack  )";
    res += "3_-1";
    RunAndCompare(cmd, res);
    cmd += R"(S1 S4 SEARCH   stack  )";
    res += "3_-1";
    RunAndCompare(cmd, res);
    cmd += R"(S1 S5 SEARCH  stack  )";
    res += "3_0";
    RunAndCompare(cmd, res);
    cmd += R"( S1 S6 SEARCH stack  )";
    res += "3_0";
    RunAndCompare(cmd, res);
}



int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

