\ ANS Forth tests - run all tests

\ Adjust the file paths as appropriate to your system
\ Select the appropriate test harness, either the simple tester.fr
\ or the more complex ttester.fs 

CR .( Running ANS Forth and Forth 2012 test programs, version 0.13.4) CR

S" ../suit1/prelimtest.fth" INCLUDED
S" ../suit1/tester.fr" INCLUDED
S" ../suit1/ttester.fS" INCLUDED

S" ../suit1/core.fr" INCLUDED
S" ../suit1/coreplustest.fth" INCLUDED
S" ../suit1/utilities.fth" INCLUDED
S" ../suit1/errorreport.fth" INCLUDED
S" ../suit1/coreexttest.fth" INCLUDED
S" ../suit1/blocktest.fth" INCLUDED
S" ../suit1/doubletest.fth" INCLUDED
S" ../suit1/exceptiontest.fth" INCLUDED
S" ../suit1/facilitytest.fth" INCLUDED
S" ../suit1/filetest.fth" INCLUDED
S" ../suit1/localstest.fth" INCLUDED
S" ../suit1/memorytest.fth" INCLUDED
S" ../suit1/toolstest.fth" INCLUDED
S" ../suit1/searchordertest.fth" INCLUDED
S" ../suit1/stringtest.fth" INCLUDED
REPORT-ERRORS

CR .( Forth tests completed ) CR CR


