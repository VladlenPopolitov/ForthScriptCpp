
#include <gtest/gtest.h>
#include <string>

#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"
#include "forthtestlib.h"

std::string ValueTo=R"(   DECIMAL 
-1 CONSTANT exc_abort 
-2 CONSTANT exc_abort" 
-13 CONSTANT exc_undef 
: t6 ABORT ;
: t10 77 SWAP ABORT" This should not be displayed" ; 
: c6 CATCH 
   CASE exc_abort OF 11 ENDOF 
        exc_abort" OF 12 ENDOF 
        exc_undef OF 13 ENDOF 
   ENDCASE 
; )";



TEST(Exceptions,AbortTest)
{
    std::string cmd = ValueTo+ R"( : stack depth  depth 0 do . loop ;  stack  )";
    std::string res = "0 ";
    RunAndCompare(cmd, res);
    /*
    DECIMAL 
-1 CONSTANT exc_abort 
-2 CONSTANT exc_abort" 
-13 CONSTANT exc_undef 
: t6 ABORT ;
The 77 in t10 is necessary for the second ABORT" test as the data stack is restored to a depth of 2 when THROW is executed. 
The 77 ensures the top of stack value is known for the results check.

: t10 77 SWAP ABORT" This should not be displayed" ; 
: c6 CATCH 
   CASE exc_abort OF 11 ENDOF 
        exc_abort" OF 12 ENDOF 
        exc_undef OF 13 ENDOF 
   ENDCASE 
;
T{ 6 7 ' T9 C6 3 -> 6 7 13 3 }T 

T{ 1 2 '  t6 c6 -> 1 2 11  }T    \ Test that ABORT is caught 
T{ 3 0 ' t10 c6 -> 3 77    }T    \ ABORT" does nothing 
T{ 4 5 ' t10 c6 -> 4 77 12 }T    \ ABORT" caught, no message
    */


    cmd += R"(  1 2 ' T6 C6 stack )";
    res += "3 11 2 1 ";
    RunAndCompare(cmd, res);
    cmd += R"(  3 0 ' T10 C6  stack  )";
    res += "2 77 3 ";
    RunAndCompare(cmd, res);
    cmd += R"(  4 5 ' T10 C6  stack  )";
    res += "3 12 77 4 ";
}

TEST(Exceptions,ThrowtTest)
{
    std::string cmd = ValueTo+ R"(DECIMAL : stack depth  depth 0 do . loop ;  stack  )";
    std::string res = "0 ";
    RunAndCompare(cmd, res);
    /*
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
    cmd += R"( : t1 9 ; : c1 1 2 3 ['] t1 CATCH ; c1 stack )";
    res += "5 0 9 3 2 1 ";
    RunAndCompare(cmd, res);
    cmd += R"(: t2 8 0 THROW ; 
: c2 1 2 ['] t2 CATCH ; c2 stack  )";
    res += "4 0 8 2 1 ";
    RunAndCompare(cmd, res);
    cmd += R"(: t3 7 8 9 99 THROW ; 
: c3 1 2 ['] t3 CATCH ; c3    stack  )";
    res += "3 99 2 1 ";
    RunAndCompare(cmd, res);
    cmd += R"(: t4 1- DUP 0> IF RECURSE ELSE 999 THROW -222 THEN ; 
: c4 3 4 5 10 ['] t4 CATCH -111 ;  c4    stack  )";
    res += "6 -111 999 0 5 4 3 ";
    RunAndCompare(cmd, res);
    cmd += R"(: t5 2DROP 2DROP 9999 THROW ; 
: c5 1 2 3 4 ['] t5 CATCH DEPTH >R DROP 2DROP 2DROP R> ;   c5    stack  )";
    res += "1 5 ";
    RunAndCompare(cmd, res);
}

std::string ExceptionTest=R"(
-1  CONSTANT EXC_ABORT
-2  CONSTANT EXC_ABORT"
-13 CONSTANT EXC_UNDEF
: T6 ABORT ;

\ The 77 in T10 is necessary for the second ABORT" test as the data stack
\ is restored to a depth of 2 when THROW is executed. The 77 ensures the top
\ of stack value is known for the results check

: T10 77 SWAP ABORT" This should not be displayed" ;
: C6 CATCH
   >R   R@ EXC_ABORT  = IF 11
   ELSE R@ EXC_ABORT" = IF 12
   ELSE R@ EXC_UNDEF  = IF 13
   THEN THEN THEN R> DROP
;
)";


TEST(Exceptions,AbortQuoteTest)
{
    std::string cmd = ExceptionTest+ R"(DECIMAL : stack depth  depth 0 do . loop ;  stack  )";
    std::string res = "0 ";
    RunAndCompare(cmd, res);
    /*
T{ 1 2 ' T6 C6  -> 1 2 11 }T     \ Test that ABORT is caught
T{ 3 0 ' T10 C6 -> 3 77 }T       \ ABORT" does nothing
T{ 4 5 ' T10 C6 -> 4 77 12 }T    \ ABORT" caught, no message
    
    */
    cmd += R"( 1 2 ' T6 C6 stack )";
    res += "3 11 2 1 ";
    RunAndCompare(cmd, res);
    cmd += R"(3 0 ' T10 C6 stack  )";
    res += "2 77 3 ";
    RunAndCompare(cmd, res);
    
    cmd += R"(4 5 ' T10 C6 stack  )";
    res += "3 12 77 4 ";
    RunAndCompare(cmd, res);
    
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

