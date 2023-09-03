\ ANS Forth tests - run all tests

\ Adjust the file paths as appropriate to your system
\ Select the appropriate test harness, either the simple tester.fr
\ or the more complex ttester.fs 

CR .( Running ANS Forth and Forth 2012 test programs, version 0.13.4) CR

S" prelimtest.fth" INCLUDED
S" tester.fr" INCLUDED



: test_util S" utilities.fth" [']  INCLUDED  CATCH  . CR ; test_util 
: test_error S" errorreport.fth" [']  INCLUDED  CATCH  . CR ; test_error 

\ S" ttester.fs" INCLUDED

: test_core S" core.fr" [']  INCLUDED  CATCH  . CR ; test_core 
\ S" coreplustest.fth" INCLUDED
\ S" utilities.fth" INCLUDED
: test-ce S" coreexttest.fth" [']  INCLUDED  CATCH  . CR ; test-ce 
 \ S" blocktest.fth" INCLUDED
: test_double S" doubletest.fth" ['] INCLUDED CATCH S" doubletest  RES " TYPE . CR ; test_double 
: test_exception S" exceptiontest.fth" ['] INCLUDED CATCH S" exceptiontest  RES " TYPE . CR ; test_exception 
: test_facility S" facilitytest.fth" ['] INCLUDED CATCH S" facilitytest  RES " TYPE . CR ; test_facility

: test_file S" filetest.fth" ['] INCLUDED CATCH S" filetest  RES " TYPE . CR ; test_file
 \ S" localstest.fth" INCLUDED
: test_memory S" memorytest.fth" ['] INCLUDED CATCH S" memorytest  RES " TYPE . CR ; test_memory
: test_tools S" toolstest.fth" ['] INCLUDED CATCH S" toolstest  RES " TYPE . CR ; test_tools
\ S" searchordertest.fth" INCLUDED
\ S" stringtest.fth" INCLUDED


REPORT-ERRORS

CR .( Forth tests completed ) CR CR


\ To run Floating Point tests

cr .( Running FP Tests) cr

s" [undefined]" pad c! pad char+ pad c@ move 
pad find nip 0=
[if]
   : [undefined]  ( "name" -- flag )
      bl word find nip 0=
   ; immediate
[then]

s" ttester.fs"         included
s" ./fp/ak-fp-test.fth"     included
s" ./fp/fatan2-test.fs"     included
s" ./fp/ieee-arith-test.fs" included
s" ./fp/ieee-fprox-test.fs" included
s" ./fp/fpzero-test.4th"    included
s" ./fp/fpio-test.4th"      included
s" ./fp/to-float-test.4th"  included
s" ./fp/paranoia.4th"       included

cr cr 
.( FP tests finished) cr cr
