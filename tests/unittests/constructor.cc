
#include <gtest/gtest.h>
#include <string>

#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"
#include "forthtestlib.h"


TEST(BasicFunctions, constructorTest)
{
    try 
    {
			std::string error = "No error\n";
			std::string res{};

 				cppforth::Forth forth{};
				forth.SetExecutionInputBuffer("6 7 8 9 .s");
                forth.ExecuteString("1 2 3 4 5  .S 2 roll .s . . . .  1 .");
				res = forth.ExecutionOutput();
				forth.ExecutionOutputReset();
				std::string res2 = forth.ExecutionOutput();
    }
    catch(...)
    {
       ADD_FAILURE() << "Uncaught exception : " << "This is OK on OS X";
    }
}

TEST(BasicFunctions,Forth001Dot)
{
    RunAndCompare("1 10 100 1000 . . . .", "1000100101");
}
TEST(BasicFunctions,Forth002DotS)
{
    RunAndCompare("1 10 100 1000 .S", "<4> 1 10 100 1000 ");
}
TEST(BasicFunctions,Forth003PlusMinusMulDiv)
{
    RunAndCompare("1 10 100 1000 10000 +  -  *  1  / .", "-109000");
}
TEST(BasicFunctions,Forth004Dup)
{
    RunAndCompare("1 dup dup + + 1+ .", "4");
}
TEST(BasicFunctions,Forth005DepthDropPickRoll)
{
    RunAndCompare("1 1 depth .", "2");
    RunAndCompare("1 2 drop .", "1");
    RunAndCompare("1 2 3 4 3 pick . . . . .", "14321");
    RunAndCompare("1 2 3 4 3 roll . . . .", "1432");
    RunAndCompare("1 2 >R . R> .", "12");
    RunAndCompare("1 2 >R . R@ .", "12");
    RunAndCompare("1 2 3 rot . . .", "132");
}
TEST(BasicFunctions,Forth006String)
{
    RunAndCompare(": a s\" literal string  \" DEPTH  . .  ; a", "216");

}
TEST(BasicFunctions,Forth006StringOutput)
{
    RunAndCompare(R"(   : pb1 CR ." You should see 2345: " ." 2345" ; pb1 depth . )", 
        "\n"                            // CR
        "You should see 2345: "         // first message
        "2345"                          // second message
        "0"                             // stack size after DEPTH .
    );
}

TEST(BasicFunctions,Forth008Loop1)
{
    RunAndCompare("1 2 3 .S >R >R >R R@ . R> . R> . R> .", "<3> 1 2 3 1123");

}
TEST(BasicFunctions,Forth008Loop2)
{
    std::string  cmd = R"(0 INVERT CONSTANT MAX-UINT 
0 INVERT 1 RSHIFT CONSTANT MAX-INT
0 INVERT 1 RSHIFT INVERT CONSTANT MIN-INT
0 INVERT 1 RSHIFT CONSTANT MID-UINT
0 INVERT 1 RSHIFT INVERT CONSTANT MID-UINT+1 : GD1 DO I LOOP ; )";
    std::string  res= "";
    RunAndCompare(cmd, res);
    cmd += " 4 1 GD1 . . . ";
    res += "321";
    RunAndCompare(cmd, res);
    cmd += " 2 -1 GD1  . . . ";
    res += "10-1";
    RunAndCompare(cmd, res);
    cmd += " MID-UINT+1 MID-UINT GD1  MID-UINT = . ";
    res += "-1";
    RunAndCompare(cmd, res);


}
TEST(BasicFunctions,Forth008Loop3)
{
    RunAndCompare(": a 3 0 do i . 2 0 do i . loop loop ; a ", "001101201");

}
TEST(BasicFunctions,Forth008Loop4)
{
    std::string  cmd = R"(0 INVERT CONSTANT MAX-UINT 
0 INVERT 1 RSHIFT CONSTANT MAX-INT
0 INVERT 1 RSHIFT INVERT CONSTANT MIN-INT
0 INVERT 1 RSHIFT CONSTANT MID-UINT
0 INVERT 1 RSHIFT INVERT CONSTANT MID-UINT+1  : GD2 DO I -1 +LOOP ;  )";
    std::string  res = "";
    RunAndCompare(cmd, res);
    cmd += " 1          4 GD2 depth . . . . . ";
    res += "41234";
    RunAndCompare(cmd, res);
    cmd += " -1 2 GD2 depth . . . . . ";
    res += "4-1012";
    RunAndCompare(cmd, res);
    cmd += " MID-UINT MID-UINT+1 GD2 MID-UINT =  swap MID-UINT+1 = . . ";
    res += "-1-1";
    RunAndCompare(cmd, res);
    cmd += R"(VARIABLE gditerations 
VARIABLE gdincrement
: gd7 ( limit start increment -- ) gdincrement !  0 gditerations !  DO  1 gditerations +! I gditerations @ 6 = IF LEAVE THEN gdincrement @ +LOOP gditerations @ ; 
: stack depth depth 0 do . loop ;  stack )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += "   4  4  -1 gd7 stack ";
    res += "214";
    RunAndCompare(cmd, res);
    cmd += "  1  4  -1 gd7 stack";
    res += "541234";
    RunAndCompare(cmd, res);
    cmd += " 4  1  -1 gd7 stack ";
    res += "76-4-3-2-101";
    RunAndCompare(cmd, res);

}
TEST(BasicFunctions,Forth008Loop5)
{
    std::string cmd =
        "  : stack depth depth 0 do . loop ;  : GI4 BEGIN DUP 1+ DUP 5 > UNTIL ; stack ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*T{ : GI4 BEGIN DUP 1+ DUP 5 > UNTIL ; -> }T 
T{ 3 GI4 -> 3 4 5 6 }T 
T{ 5 GI4 -> 5 6 }T 
T{ 6 GI4 -> 6 7 }T*/
    cmd += " 3 GI4 stack ";
    res += "46543";
    RunAndCompare(cmd, res);
    cmd += "  5 GI4 stack ";
    res += "265";
    RunAndCompare(cmd, res);
    cmd += "  6 GI4 stack ";
    res += "276";
    RunAndCompare(cmd, res);

}
/*T{ : GI5 BEGIN DUP 2 > WHILE DUP 5 < WHILE DUP 1+ REPEAT IF 123 ELSE 345 THEN ; -> }T
T{ 1 GI5 -> 1 345 }T
T{ 2 GI5 -> 2 345 }T
T{ 3 GI5 -> 3 4 5 123 }T
T{ 4 GI5 -> 4 5 123 }T
T{ 5 GI5 -> 5 123 }T
*/
TEST(BasicFunctions,Forth008Loop6)
{
    std::string cmd =
": stack depth depth 0 do . loop ; : GI5 BEGIN DUP 2 > WHILE DUP 5 < WHILE DUP 1 + REPEAT 123 ELSE 345 THEN ; stack ";
    std::string res = "0";
    RunAndCompare(cmd, res);

    cmd += " 1 GI5 stack ";
    res += "23451";
    RunAndCompare(cmd, res);
    cmd += "  2 GI5 stack ";
    res += "23452";
    RunAndCompare(cmd, res);
    cmd += "  3 GI5 stack ";
    res += "4123543";
    RunAndCompare(cmd, res);
    cmd += "  4 GI5 stack ";
    res += "312354";
    RunAndCompare(cmd, res);
    cmd += "  5 GI5 stack ";
    res += "21235";
    RunAndCompare(cmd, res);

}
TEST(BasicFunctions,Forth008Loop7unloop)
{
    /*
    T{ : GD6  ( PAT: T{0 0},{0 0}{1 0}{1 1},{0 0}{1 0}{1 1}{2 0}{2 1}{2 2} ) 0 SWAP 0 DO I 1+ 0 DO I J + 3 = IF I UNLOOP I UNLOOP EXIT THEN 1+ LOOP    LOOP ; -> }T
    T{ 1 GD6 -> 1 }T
    T{ 2 GD6 -> 3 }T
    T{ 3 GD6 -> 4 1 2 }T

    */
    std::string cmd =
        ": stack depth depth 0 do . loop ;  : GD6 0 SWAP 0 DO I 1+ 0 DO I J + 3 = IF I UNLOOP I UNLOOP EXIT THEN 1+ LOOP    LOOP  ; stack ";
    std::string res = "0";
    RunAndCompare(cmd, res);

    cmd += " 3 GD6 stack ";
    res += "3214";
    RunAndCompare(cmd, res, true);
    cmd += "  ";
    res += "";
    RunAndCompare(cmd, res);

}
TEST(BasicFunctions,Forth008Loop8_question_do)
{
    /*
    : qd4 ?DO I -1 +LOOP ;
    T{  4 4 qd4 -> }T
    T{  1 4 qd4 -> 4 3 2  1 }T
    T{ -1 2 qd4 -> 2 1 0 -1 }T
    */
    std::string cmd =
        ": stack depth depth 0 do . loop ; : qd4 ?DO I -1 +LOOP ;  stack ";
    std::string res = "0";
    RunAndCompare(cmd, res);

    cmd += "  1 4 qd4 stack ";
    res += "41234";
    RunAndCompare(cmd, res, true);
    cmd += " -1 2 qd4 stack ";
    res += "4-1012";
    RunAndCompare(cmd, res);

}
TEST(BasicFunctions,Forth008Loop9_do_loop)
{
    /*
    MAX-UINT 8 RSHIFT 1+ CONSTANT USTEP
    USTEP NEGATE CONSTANT -USTEP
    MAX-INT 7 RSHIFT 1+ CONSTANT STEP
    STEP NEGATE CONSTANT -STEP

    VARIABLE BUMP

    T{ : GD8 BUMP ! DO 1+ BUMP @ +LOOP ; -> }T

    T{ 0 MAX-UINT 0 USTEP GD8 -> 256 }T
    T{ 0 0 MAX-UINT -USTEP GD8 -> 256 }T

    T{ 0 MAX-INT MIN-INT STEP GD8 -> 256 }T
    T{ 0 MIN-INT MAX-INT -STEP GD8 -> 256 }T

    \ Two's complement arithmetic, wraps around modulo wordsize
    \ Only tested if the Forth system does wrap around, use of conditional
    \ compilation deliberately avoided

    MAX-INT 1+ MIN-INT = CONSTANT +WRAP?
    MIN-INT 1- MAX-INT = CONSTANT -WRAP?
    MAX-UINT 1+ 0=       CONSTANT +UWRAP?
    0 1- MAX-UINT =      CONSTANT -UWRAP?
    */
    std::string cmd = R"(0 INVERT CONSTANT MAX-UINT 
0 INVERT 1 RSHIFT CONSTANT MAX-INT
0 INVERT 1 RSHIFT INVERT CONSTANT MIN-INT
0 INVERT 1 RSHIFT CONSTANT MID-UINT
0 INVERT 1 RSHIFT INVERT CONSTANT MID-UINT+1  )"
        " : stack depth depth 0 do . loop ; "
        " MAX-UINT 8 RSHIFT 1+ CONSTANT USTEP USTEP NEGATE CONSTANT -USTEP "
        " MAX-INT 7 RSHIFT 1+ CONSTANT STEP STEP NEGATE CONSTANT -STEP "
        " VARIABLE BUMP "
        " : GD8 BUMP ! DO 1+ BUMP @ +LOOP ; "
        "  stack ";
    std::string res = "0";
    RunAndCompare(cmd, res);

    //cmd += " -1 2 qd4 stack ";
    //res += "4-1012";
    //RunAndCompare(cmd, res);

}
TEST(BasicFunctions,Forth009Constant)
{
    std::string cmd =

        " 1234 CONSTANT CONSTEST DEPTH CONSTEST + .";

    RunAndCompare(cmd, "1234");

}
TEST(BasicFunctions,Forth010Defined)
{
    std::string cmd =

        " : ?DEFINED  ( \"name\" -- 0 | -1 )  32 WORD FIND SWAP DROP 0= 0= ; ?DEFINED SWAP .";

    RunAndCompare(cmd, "-1");

}
TEST(BasicFunctions,Forth011Immediate)
{
    std::string cmd =

        "VARIABLE TIMM1 0 TIMM1 ! : TIMM2  123 TIMM1 ! ; IMMEDIATE : TIMM3 TIMM2 ; TIMM1 @ 123 = .";
    std::string res = "-1";
    RunAndCompare(cmd, res);
    /*
    T{ 123 CONSTANT iw1 IMMEDIATE iw1 -> 123 }T
    T{ : iw2 iw1 LITERAL ; iw2 -> 123 }T
    T{ VARIABLE iw3 IMMEDIATE 234 iw3 ! iw3 @ -> 234 }T
    T{ : iw4 iw3 [ @ ] LITERAL ; iw4 -> 234 }T

    T{ :NONAME [ 345 ] iw3 [ ! ] ; DROP iw3 @ -> 345 }T
    T{ CREATE iw5 456 , IMMEDIATE -> }T
    T{ :NONAME iw5 [ @ iw3 ! ] ; DROP iw3 @ -> 456 }T

    T{ : iw6 CREATE , IMMEDIATE DOES> @ 1+ ; -> }T
    T{ 111 iw6 iw7 iw7 -> 112 }T
    T{ : iw8 iw7 LITERAL 1+ ; iw8 -> 113 }T

    T{ : iw9 CREATE , DOES> @ 2 + IMMEDIATE ; -> }T
    : find-iw BL WORD FIND NIP ;
    T{ 222 iw9 iw10 find-iw iw10 -> -1 }T    \ iw10 is not immediate
    T{ iw10 find-iw iw10 -> 224 1 }T          \ iw10 becomes immediate
    */
    cmd = "123 CONSTANT iw1 IMMEDIATE iw1 dup .";
    res = "123";
    RunAndCompare(cmd, res);
    cmd += " : iw2 iw1 LITERAL ; iw2 dup .";
    res += "123";
    RunAndCompare(cmd, res);
    cmd += " VARIABLE iw3 IMMEDIATE 234 iw3 ! iw3 @ dup .";
    res += "234";
    RunAndCompare(cmd, res);
    cmd += " : iw4 iw3 [ @ ] LITERAL ; iw4 dup . ";
    res += "234";
    RunAndCompare(cmd, res);
    cmd += " :NONAME [ 345 ] iw3 [ ! ] ; DROP iw3 @ dup . ";
    res += "345";
    RunAndCompare(cmd, res);
    cmd += "  CREATE iw5 456 , IMMEDIATE  ";
    res += "";
    RunAndCompare(cmd, res);
    cmd += "  :NONAME iw5 [ @ iw3 ! ] ; DROP iw3 @ dup . ";
    res += "456";
    RunAndCompare(cmd, res);
    cmd += " : iw6 CREATE , IMMEDIATE DOES> @ 1+ ; ";
    res += "";
    RunAndCompare(cmd, res);
    cmd += "  111 iw6 iw7 iw7 dup . \n";
    res += "112";
    RunAndCompare(cmd, res);
    cmd += ": iw8 iw7 LITERAL 1+ ; iw8 dup .";
    res += "113";
    RunAndCompare(cmd, res);
    cmd += " : iw9 CREATE , DOES> @ 2 + IMMEDIATE ; : find-iw BL WORD FIND NIP ; ";
    res += "";
    RunAndCompare(cmd, res);
    cmd += " 222 iw9 iw10 find-iw iw10 dup . ";
    res += "-1";
    RunAndCompare(cmd, res);
    //cmd += " iw10 find-iw iw10 . . ";
    cmd += " iw10 find-iw iw10  . . ";
    res += "1224";
    RunAndCompare(cmd, res);


};
TEST(BasicFunctions,Forth012DoesMore)
{
    std::string cmd = ": DOES1 DOES> @ 1 + ;  : DOES2 DOES> @ 2 + ; ";
    std::string res = "";
    RunAndCompare(cmd, res);
    cmd += "  CREATE CR1  "; res += "";
    RunAndCompare(cmd, res);
    /*
    T{ : DOES1 DOES> @ 1 + ; -> }T
    T{ : DOES2 DOES> @ 2 + ; -> }T
    T{ CREATE CR1 -> }T
    T{ CR1   -> HERE }T 
    T{ 1 ,   ->   }T
    T{ CR1 @ -> 1 }T
    T{ DOES1 ->   }T
    T{ CR1   -> 2 }T
    T{ DOES2 ->   }T
    T{ CR1   -> 3 }T
    T{ : WEIRD: CREATE DOES> 1 + DOES> 2 + ; -> }T
    T{ WEIRD: W1 -> }T
    T{ ' W1 >BODY -> HERE }T
    T{ W1 -> HERE 1 + }T
    T{ W1 -> HERE 2 + }T*/
    cmd += "  CR1 dup here = .  "; res += "-1";
    RunAndCompare(cmd, res);
    cmd += " 1 ,  CR1 @ dup .  "; res += "1";
    RunAndCompare(cmd, res);
    cmd += " DOES1 CR1 dup . "; res += "2";
    RunAndCompare(cmd, res);
    cmd += " DOES2 CR1 dup . "; res += "3";
    RunAndCompare(cmd, res);
    cmd += " : WEIRD: CREATE DOES> 1 + DOES> 2 + ;  WEIRD: W1  "; res += "";
    RunAndCompare(cmd, res);
    cmd += "  ' W1 >BODY dup here = . "; res += "-1";
    RunAndCompare(cmd, res);
    cmd += "  W1 dup here 1 + = . "; res += "-1";
    RunAndCompare(cmd, res);
    cmd += "   W1 dup here 2 + =  . "; res += "-1";
    RunAndCompare(cmd, res);
    cmd += "  "; res += "";
    RunAndCompare(cmd, res);
    cmd += "  "; res += "";
    RunAndCompare(cmd, res);
    cmd += "  "; res += "";
    RunAndCompare(cmd, res);

};
TEST(BasicFunctions,Forth013IfElseThen)
{
    std::string cmd = "  : GI1 IF 123 THEN ;  : GI2 IF 123 ELSE 234 THEN ;  : stack depth depth 0 do . loop ;  stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    cmd += "   0 GI1 stack  "; res += "0";
    RunAndCompare(cmd, res);
    /*
    T{  1 GI1 -> 123 }T
    T{ -1 GI1 -> 123 }T
    T{  0 GI2 -> 234 }T
    T{  1 GI2 -> 123 }T
    T{ -1 GI1 -> 123 }T
    \ Multiple ELSEs in an IF statement
    : melse IF 1 ELSE 2 ELSE 3 ELSE 4 ELSE 5 THEN ;
    T{ <FALSE> melse -> 2 4 }T
    T{ <TRUE>  melse -> 1 3 5 }T
    */
    cmd += "   1 GI1 stack  "; res += "1123";
    RunAndCompare(cmd, res);
    cmd += " -1 GI1 stack  "; res += "1123";
    RunAndCompare(cmd, res);
    cmd += "  0 GI2 stack "; res += "1234";
    RunAndCompare(cmd, res);
    cmd += "  1 GI2 stack "; res += "1123";
    RunAndCompare(cmd, res);
    cmd += " -1 GI1  stack  "; res += "1123";
    RunAndCompare(cmd, res);
    cmd += " : melse IF 1 ELSE 2 ELSE 3 ELSE 4 ELSE 5 THEN ; stack "; res += "0";
    RunAndCompare(cmd, res);
    cmd += " false melse stack "; res += "242";
    RunAndCompare(cmd, res);
    cmd += "  true  melse stack "; res += "3531";
    RunAndCompare(cmd, res);
    cmd += "   : pt1 AHEAD 1111 2222 THEN 3333 ;  pt1 stack "; res += "13333";
    RunAndCompare(cmd, res);


};
// >NUMBER
TEST(BasicFunctions,Forth014toNumber)
{
    std::string cmd = R"( : GN1   \ ( UD BASE -- UD' LEN ) UD SHOULD EQUAL UD' AND LEN SHOULD BE ZERO.
BASE @ >R BASE !
<# #S #>
0 0 2SWAP >NUMBER SWAP DROP      \ RETURN LENGTH ONLY
R> BASE ! ; HEX 24 CONSTANT MAX-BASE  0 INVERT CONSTANT MAX-UINT  : stack depth depth 0 do . loop ;  stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
//			cmd += " 0 0 2 GN1 stack "; res += "3000";
//			RunAndCompare(cmd, res);
    /*
    T{ 0 0 2 GN1 -> 0 0 0 }T
    T{ MAX-UINT 0 2 GN1 -> MAX-UINT 0 0 }T
    T{ MAX-UINT DUP 2 GN1 -> MAX-UINT DUP 0 }T
    T{ 0 0 MAX-BASE GN1 -> 0 0 0 }T
    T{ MAX-UINT 0 MAX-BASE GN1 -> MAX-UINT 0 0 }T
    T{ MAX-UINT DUP MAX-BASE GN1 -> MAX-UINT DUP 0 }T
    */
    cmd += " MAX-UINT DUP MAX-BASE GN1 stack "; res += "30ffffffffffffffff"; 
    RunAndCompare(cmd, res);
   
};

TEST(BasicFunctions,Forth015Source)
{
    std::string cmd = R"( : GS1 S" SOURCE" 2DUP EVALUATE >R SWAP >R = R> R> = ; : GS4 SOURCE >IN ! DROP ;  : stack depth depth 0 do . loop ;  stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    cmd += " GS4 123 456 \nstack "; res += "0";
    RunAndCompare(cmd, res);
    cmd += " GS1 \nstack "; res += "2-1-1"; 
    RunAndCompare(cmd, res);
   
};
TEST(BasicFunctions,Forth016MemoryAllocateFree)
{
    std::string cmd = R"(  : stack depth depth 0 do . loop ; variable addr VARIABLE datsp HERE datsp ! stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    //			cmd += " 0 0 2 GN1 stack "; res += "3000";
    //			RunAndCompare(cmd, res);
    /*
    VARIABLE datsp
    HERE datsp !
    T{ 50 CELLS ALLOCATE SWAP addr ! -> 0 }T
    T{ addr @ ALIGNED -> addr @ }T	   \ Test address is aligned
    T{ HERE -> datsp @ }T	             \ Check data space pointer is unaffected
    addr @ 50 write-cell-mem
    addr @ 50 check-cell-mem         \ Check we can access the heap
    T{ addr @ FREE -> 0 }T

    T{ 99 ALLOCATE SWAP addr ! -> 0 }T
    T{ addr @ ALIGNED -> addr @ }T    \ Test address is aligned
    T{ addr @ FREE -> 0 }T
    T{ HERE -> datsp @ }T	             \ Data space pointer unaffected by FREE
    T{ -1 ALLOCATE SWAP DROP 0= -> <FALSE> }T    \ Memory allocate failed
    */
    cmd += " 50 CELLS ALLOCATE SWAP addr ! stack "; res += "10"; // 42949672954294967295
    RunAndCompare(cmd, res);
    cmd += " addr @ ALIGNED addr @ - stack "; res += "10";
    RunAndCompare(cmd, res);
    cmd += "  HERE datsp @ - stack "; res += "10";
    RunAndCompare(cmd, res);
    cmd += "  addr @ FREE stack "; res += "10";
    RunAndCompare(cmd, res);


};
TEST(BasicFunctions,Forth016MemoryResize)
{
    std::string cmd = R"(  : stack depth depth 0 do . loop ; variable addr  VARIABLE datsp HERE datsp !  stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    cmd += " 50 CHARS ALLOCATE SWAP addr !  stack "; res += "10";
    RunAndCompare(cmd, res);
    /*
    T{ 50 CHARS ALLOCATE SWAP addr ! -> 0 }T
    addr @ 50 write-char-mem addr @ 50 check-char-mem
    \ Resize smaller does not change content.
    T{ addr @ 28 CHARS RESIZE SWAP addr ! -> 0 }T
    addr @ 28 check-char-mem

    \ Resize larger does not change original content.
    T{ addr @ 100 CHARS RESIZE SWAP addr ! -> 0 }T
    addr @ 28 check-char-mem

    \ Resize error does not change addr
    T{ addr @ -1 RESIZE 0= -> addr @ <FALSE> }T

    T{ addr @ FREE -> 0 }T
    T{ HERE -> datsp @ }T

    */
    cmd += "  addr @ 28 CHARS RESIZE SWAP addr !  stack "; res += "10"; // 42949672954294967295
    RunAndCompare(cmd, res);
    cmd += "  addr @ 100 CHARS RESIZE SWAP addr !  stack "; res += "10";
    RunAndCompare(cmd, res);
    cmd += "  addr @ -1 RESIZE 0= swap  addr @ -   stack "; res += "200";
    RunAndCompare(cmd, res);
    cmd += "  addr @ FREE HERE datsp @ - stack "; res += "200";
    RunAndCompare(cmd, res);


};
TEST(BasicFunctions,Forth017Comment)
{
    std::string cmd = R"(  : stack depth depth 0 do . loop ;  stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
                cmd += "  : pc1 ( A comment)1234 ; pc1    stack "; res += "11234";
                RunAndCompare(cmd, res);
    /*
    T{ ( 1 2 3
    4 5 6
    7 8 9 ) 11 22 33 -> 11 22 33 }T
    */
                RunAndCompare(cmd, res);
                cmd += R"(   ( 1 2 3
4 5 6
7 8 9 ) 11 22 33
stack )"; res += "3332211";
                RunAndCompare(cmd, res);

};
TEST(BasicFunctions,Forth018Execute)
{
    std::string cmd = "  : stack depth depth 0 do . loop ;  stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    cmd += "  : GT1 123 ;  : GT2 [\'] GT1 ; IMMEDIATE   stack "; res += "0";
    RunAndCompare(cmd, res);
    /*
    F.6.1.0070'
    T{ : GT1 123 ;   ->     }T
    T{ ' GT1 EXECUTE -> 123 }T
    F.6.1.2510[']
    T{ : GT2 ['] GT1 ; IMMEDIATE -> }T
    T{ GT2 EXECUTE -> 123 }T
    */
    RunAndCompare(cmd, res);
    cmd += "  \' GT1 EXECUTE stack "; res += "1123";
    RunAndCompare(cmd, res);
    cmd += " GT2 EXECUTE stack "; res += "1123";
    RunAndCompare(cmd, res);

};
TEST(BasicFunctions,Forth019Catch)
{
    std::string cmd = "  : stack depth depth 0 do . loop ;  stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    cmd += R"( DECIMAL : t1 9 ; : c1 1 2 3 ['] t1 CATCH ;   stack )"; res += "0";
    RunAndCompare(cmd, res);
    /*
    F.9.6.1.2275THROW
    DECIMAL
    : t1 9 ;
    : c1 1 2 3 ['] t1 CATCH ;
    T{ c1 -> 1 2 3 9 0 }T    \ No THROW executed
    : t2 8 0 THROW ;
    : c2 1 2 ['] t2 CATCH ;
    T{ c2 -> 1 2 8 0 }T    \ 0 THROW does nothing
    : t3 7 8 9 99 THROW ;
    : c3 1 2 ['] t3 CATCH ;
    T{ c3 -> 1 2 99 }T    \ Restores stack to CATCH depth
    : t4 1- DUP 0> IF RECURSE ELSE 999 THROW -222 THEN ;
    : c4 3 4 5 10 ['] t4 CATCH -111 ;
    T{ c4 -> 3 4 5 0 999 -111 }T        \ Test return stack unwinding
    : t5 2DROP 2DROP 9999 THROW ;
    : c5 1 2 3 4 ['] t5 CATCH           \ Test depth restored correctly
    DEPTH >R DROP 2DROP 2DROP R> ;    \ after stack has been emptied
    T{ c5 -> 5 }T
    */
    cmd += R"(  c1  stack )"; res += "509321";
    RunAndCompare(cmd, res);
    cmd += R"( : t2 8 0 THROW ; : c2 1 2 ['] t2 CATCH ; c2 stack )"; res += "40821";
    RunAndCompare(cmd, res);
    cmd += R"( : t3 7 8 9 99 THROW ; : c3 1 2 ['] t3 CATCH ; c3 stack )"; res += "39921";
    RunAndCompare(cmd, res);
    cmd += R"( : t4 1- DUP 0> IF RECURSE ELSE 999 THROW -222 THEN ; : c4 3 4 5 10 ['] t4 CATCH -111 ; c4 stack )"; 
    res += "6-1119990543";
    RunAndCompare(cmd, res);
    cmd += R"( : t5 2DROP 2DROP 9999 THROW ; : c5 1 2 3 4 ['] t5 CATCH DEPTH >R DROP 2DROP 2DROP R> ; c5 stack )"; 
    res += "15";
    RunAndCompare(cmd, res);


};
TEST(BasicFunctions,Forth020Float001)
{
    std::string cmd = "  : stack depth depth 0 do . loop ;  stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    cmd += R"( : f1 3 0 D>F  2 0 D>F  1 0 D>F FOVER FDEPTH FMIN FMAX FSWAP F>D F>D FDEPTH ; f1   stack )";
    res += "6002034";
    RunAndCompare(cmd, res);
    cmd += R"( : f1 5 0 D>F  3 0 D>F F/ FDUP FLOOR F>D FDUP FTRUNC F>D FDUP FROUND F>D FNEGATE  FDUP FLOOR F>D FDUP FTRUNC F>D FDUP FROUND F>D FDROP  ; f1   stack )";
    res += "12-1-2-1-1-1-2020101";
    RunAndCompare(cmd, res);
    cmd += R"( : f1 2 FLOATS FLOAT+ 1 FLOATS / ; f1   stack )"; res += "13";
    RunAndCompare(cmd, res);
    cmd += R"( : f1  FALIGN HERE DUP FALIGNED = if 333 else 444 then  3 ALLOT HERE DUP  FALIGNED = if 555 else 666 then ; f1   stack )"; res += "2666333";
    RunAndCompare(cmd, res);
    cmd += R"( : f1 1 0 D>F  FDUP FDUP F+ F< if 333 else 444 then     ; f1   stack )"; res += "1333";
    RunAndCompare(cmd, res);
    cmd += R"( : f1 1 0 D>F  F0= if 0 else 1 then  0 0 D>F  F0= if 5 else 6 then   ; f1   stack )"; res += "251";
    RunAndCompare(cmd, res);
    cmd += R"( : f1 1 0 D>F FDUP F<0 if 0 else 1 then FDEPTH FDROP ; f1 stack )"; res += "211";
    RunAndCompare(cmd, res);
    cmd += R"( 1 0 D>F FDUP F+ FDUP F* FDUP F/ FDUP F- F>D FDEPTH stack )"; res += "3000";
    RunAndCompare(cmd, res);
    cmd += R"(   stack )"; res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(   stack )"; res += "0";
    RunAndCompare(cmd, res);


};
TEST(BasicFunctions,Forth020Float003Fliteral)
{
    std::string cmd = "  : stack depth depth 0 do . loop ;  stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    cmd += "    stack  ";
    res += "0";
    RunAndCompare(cmd, res);
}
TEST(BasicFunctions,Forth021Compare)
{
    std::string cmd = " HERE 255 ALLOT constant pad : stack depth depth 0 do . loop ; : s1 S\" abcdefghaaa \" ; stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
    T{ s1        s1 COMPARE ->  0  }T 
T{ s1  PAD SWAP CMOVE   ->     }T    \ Copy s1 to PAD 
T{ s1  PAD OVER COMPARE ->  0  }T 
T{ s1     PAD 6 COMPARE ->  1  }T 
T{ PAD 10    s1 COMPARE -> -1  }T 
T{ s1     PAD 0 COMPARE ->  1  }T 
T{ PAD  0    s1 COMPARE -> -1  }T 
T{ s1        s6 COMPARE ->  1  }T 
T{ s6        s1 COMPARE -> -1  }T
: "abdde" S" abdde" ; 
: "abbde" S" abbde" ; 
: "abcdf" S" abcdf" ; 
: "abcdee" S" abcdee" ;

T{ s1 "abdde"  COMPARE -> -1 }T 
T{ s1 "abbde"  COMPARE ->  1 }T 
T{ s1 "abcdf"  COMPARE -> -1 }T 
T{ s1 "abcdee" COMPARE ->  1 }T

: s11 S" 0abc" ; 
: s12 S" 0aBc" ;

T{ s11 s12 COMPARE ->  1 }T 
T{ s12 s11 COMPARE -> -1 }T
    */
    cmd += R"(  s1        s1 COMPARE    stack  )";
    res += "10";
    RunAndCompare(cmd, res);
    cmd += R"(  s1  PAD SWAP CMOVE    stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(  s1  PAD OVER COMPARE   stack  )";
    res += "10";
    RunAndCompare(cmd, res);
    cmd += R"(  s1     PAD 6 COMPARE    stack  )";
    res += "11";
    RunAndCompare(cmd, res);
    cmd += R"(  PAD 10    s1 COMPARE    stack  )";
    res += "1-1";
    RunAndCompare(cmd, res);
    cmd += R"(  s1     PAD 0 COMPARE    stack  )";
    res += "11";
    RunAndCompare(cmd, res);
    cmd += R"(  PAD  0    s1 COMPARE    stack  )";
    res += "1-1";
    RunAndCompare(cmd, res);
    cmd += R"( : "abdde" S" abdde" ; 
: "abbde" S" abbde" ; 
: "abcdf" S" abcdf" ; 
: "abcdee" S" abcdee" ; s1 "abdde"  COMPARE    stack  )";
    res += "1-1";
    RunAndCompare(cmd, res);
    cmd += R"( : s11 S" 0abc" ; 
: s12 S" 0aBc" ;   s11 s12 COMPARE  stack  )";
    res += "11";
    RunAndCompare(cmd, res);
    cmd += R"(   s12 s11 COMPARE   stack  )";
    res += "1-1";
    RunAndCompare(cmd, res);
    /*cmd += R"(    stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(    stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(    stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(    stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(    stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    */

}

TEST(BasicFunctions,Forth022Postpone)
{
    std::string cmd = "  : stack depth depth 0 do . loop ;  stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
T{ : GT4 POSTPONE GT1 ; IMMEDIATE -> }T 
T{ : GT5 GT4 ; -> }T 
T{ GT5 -> 123 }T
T{ : GT6 345 ; IMMEDIATE -> }T 
T{ : GT7 POSTPONE GT6 ; -> }T 
T{ GT7 -> 345 }T
    */

    cmd += R"(  : GT1 123 ;  : GT4 POSTPONE GT1 ; IMMEDIATE : GT5 GT4 ;    stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(  GT5  stack  )";
    res += "1123";
    RunAndCompare(cmd, res);
    cmd += R"(  : GT6 345 ; IMMEDIATE   stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(  : GT7 POSTPONE GT6 ;    stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(   GT7  stack  )";
    res += "1345";
    RunAndCompare(cmd, res);
    

}

TEST(BasicFunctions,Forth023Evaluate)
{
    std::string cmd = "  : stack depth depth 0 do . loop ;  stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
    : GE1 S" 123" ; IMMEDIATE
    : GE2 S" 123 1+" ; IMMEDIATE
    : GE3 S" : GE4 345 ;" ;
    : GE5 EVALUATE ; IMMEDIATE
    T{ GE1 EVALUATE -> 123 }T ( TEST EVALUATE IN INTERP. STATE )
    T{ GE2 EVALUATE -> 124 }T
    T{ GE3 EVALUATE ->     }T
    T{ GE4          -> 345 }T

    T{ : GE6 GE1 GE5 ; -> }T ( TEST EVALUATE IN COMPILE STATE )
    T{ GE6 -> 123 }T
    T{ : GE7 GE2 GE5 ; -> }T
    T{ GE7 -> 124 }T
    */

    cmd += R"(  : GE1 S" 123" ; IMMEDIATE
    : GE2 S" 123 1+" ; IMMEDIATE
    : GE3 S" : GE4 345 ;" ;
    : GE5 EVALUATE ; IMMEDIATE   stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(  GE1 EVALUATE   stack  )";
    res += "1123";
    RunAndCompare(cmd, res);
    cmd += R"(  GE2 EVALUATE   stack  )";
    res += "1124";
    RunAndCompare(cmd, res);
    cmd += R"(  GE3 EVALUATE   stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(  GE4 stack  )";
    res += "1345";
    RunAndCompare(cmd, res);
    cmd += R"(   : GE6 GE1 GE5 ;  stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( GE6  stack  )";
    res += "1123";
    RunAndCompare(cmd, res);
    cmd += R"(  : GE7 GE2 GE5 ;   stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"( GE7  stack  )";
    res += "1124";
    RunAndCompare(cmd, res);



}
TEST(BasicFunctions,Forth023EvaluateCompile)
{
    std::string cmd = R"(  : stack depth depth 0 do . loop ;  : GE1 S" 123" ; IMMEDIATE : GE5 EVALUATE ; IMMEDIATE  : GE6 GE1 GE5 ;  stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
    : GE1 S" 123" ; IMMEDIATE
    : GE2 S" 123 1+" ; IMMEDIATE
    : GE3 S" : GE4 345 ;" ;
    : GE5 EVALUATE ; IMMEDIATE
    T{ GE1 EVALUATE -> 123 }T ( TEST EVALUATE IN INTERP. STATE )
    T{ GE2 EVALUATE -> 124 }T
    T{ GE3 EVALUATE ->     }T
    T{ GE4          -> 345 }T

    T{ : GE6 GE1 GE5 ; -> }T ( TEST EVALUATE IN COMPILE STATE )
    T{ GE6 -> 123 }T
    T{ : GE7 GE2 GE5 ; -> }T
    T{ GE7 -> 124 }T
    */


    cmd += R"( GE6  stack  )";
    res += "1123";
    RunAndCompare(cmd, res);




}

TEST(BasicFunctions,Forth020Float002)
{
    std::string cmd = "  : stack depth depth 0 do . loop ;  stack  ";
    std::string res = "0";
    RunAndCompare(cmd, res);
    return;
    cmd += R"( 
: f2/ 0.5  F* ;
: F. F>D drop . ;
100 floats allocate throw
constant farray

: init_farray
0 0 D>F
farray
100 0 do
dup fdup f!
float+
loop
drop fdrop
;

: my_loop
init_farray
0.0                  \ x pu
10.0                 \ x pu mu

0 do
farray
100 0 do
    2.0  f+ f2/
    dup fdup f!
    float+
loop
drop                    \ x pu mu
frot frot               \ mu x pu
fswap 0.0                \ mu pu x su
farray
100 0 do
    fover  f*
    dup f@ f+
    float+
loop
drop
frot f+                 \ mu x pu
frot                    \ x pu mu
loop
fdrop f. cr fdrop
;

0.2 500000 my_loop
)";
    res += "0";
    RunAndCompare(cmd, res);


};
TEST(BasicFunctions,Forth024Defer)
{
    std::string cmd = R"(  : stack depth depth 0 do . loop ;  DEFER defer2 stack  )";
    std::string res = "0";
    RunAndCompare(cmd, res);
    /*
    T{ DEFER defer2 ->   }T
    T{ ' * ' defer2 DEFER! -> }T
    T{   2 3 defer2 -> 6 }T
    T{ ' + IS defer2 ->   }T
    T{    1 2 defer2 -> 3 }T
    */


    cmd += R"(  ' * ' defer2 DEFER! stack )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(   2 3 defer2  stack  )";
    res += "16";
    RunAndCompare(cmd, res);
    cmd += R"(  ' + IS defer2  stack  )";
    res += "0";
    RunAndCompare(cmd, res);
    cmd += R"(   1 2 defer2  stack  )";
    res += "13";
    RunAndCompare(cmd, res);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

