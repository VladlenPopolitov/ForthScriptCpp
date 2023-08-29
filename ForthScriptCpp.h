#ifndef FORTHSCRIPTCPP
#define FORTHSCRIPTCPP

/****

ForthScriptCpp: A Forth impmlementation as C++ Class by Vladlen Popolitov
CORE is fully impelemented, all CORE tests are passed.
FILE module is implemented
MEMORY module is implemented
FLOAT and FLOATEXT modules are implemented.
An output to standart and error output streams can be disabled.
Forth words can be implemented in C++.
Forth debugger can be implemented.
==============================================

C++ Class forthcpp::Forth is inspired by Kristopher Johnson cxxforth. 
Though the most lines of code are changed to implements C++ members and virtual 
forth memory, many method names and comments are unchanged. forthcpp::Forth class
can be used in C++ program as embeded script language. Forth accesses only virtual memory
and cannot harm anything in C++ address space. Forth address space, stacks and definitions
dictionary are implemented as std::vector.

This free software released into public domain and can be used without any restrictions.

A macro `FORTHSCRIPTCPP_ENABLE_FILE` can be defined to include into ForthScriptCPP  words for opening,
reading, and writing files.  You may want to do this on a platform that supports file access.

A macro `FORTHSCRIPTCPP_ENABLE_MEMORY` can be defined to include into ForthScriptCPP  words for memory manipulation.

A macro `FORTHSCRIPTCPP_ENABLE_FLOAT` can be defined to include Float module

A macro `FORTHSCRIPTCPP_ENABLE_FLOAT_EXT` can be defined to include Float Extensions module

A macro `FORTHSCRIPTCPP_ENABLE_INTERNALDEBUGGER` to include class members to register callback function called after 
specified quantity of steps (command executions) in Forth.

A macro FORTHSCRIPTCPP_DISABLE_OUTPUT can be defined to prevent output to output stream. This feature can be useful,
if Forth language is used in ini-files, when output capabilities are not needed and even dangerous.


The rest of this comment copied from the source text of cxxforth. 
=================================================================

cxxforth: A Simple Forth Implementation in C++
==============================================

by Kristopher Johnson

<https://github.com/kristopherjohnson/cxxforth>

----

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

----

`cxxforth` is a simple implementation of [Forth][forth] in C++.

There are many examples of Forth implementations available on the Internet, but
most of them are written in assembly language or low-level C, with a focus in
maximizing efficiency and demonstrating traditional Forth implementation
techniques.  This Forth is different: My goal is to use modern C++ to create a
Forth implementation that is easy to understand, easy to port, and easy to
extend.  I'm not going to talk about register assignments or addressing modes
or opcodes or the trade-offs between indirect threaded code, direct threaded
code, subroutine threaded code, and token threaded code.  I'm just going to
build a working Forth system in a couple thousand lines of C++ and Forth.

An inspiration for this implementation is Richard W.M. Jones's
[JONESFORTH][jonesforth].  JONESFORTH is a Forth implementation written as a
very readable tutorial, and I am adopting its style for our higher-level
implementation.  This Forth kernel is written as a [C++ file](cxxforth.cpp)
with large comment blocks, and there is a utility, [cpp2md](cpp2md.cpp.md),
that takes that C++ file and converts it to a [Markdown][markdown]-format
document [cxxforth.cpp.md](cxxforth.cpp.md) with nicely formatted commentary
sections between the C++ code blocks.

As in other Forth systems, the basic design of this Forth is to create a small
kernel in native code (C++, in this case), and then implement the rest of the
system with Forth code.  The kernel has to provide the basic primitives needed
for memory access, arithmetic, logical operations, and operating system access.
With those primitives, we can then write Forth code to extend the system.

I am writing C++ conforming to the C++14 standard.  If your C++ compiler does
not support C++14 yet, you may need to make some modifications to the code to
get it built.

The Forth words provided by cxxforth are based on those in the [ANS Forth draft
standard][dpans] and [Forth 2012 standard][forth2012].  I don't claim
conformance to any standard, but you can use these standards as a crude form of
documentation for the Forth words that are implemented here.  cxxforth
implements many of the words from the standards' core word sets, and a
smattering of words from other standard word sets.

In addition to words from the standards, cxxforth provides a few non-standard
words.  Each of these is marked with "Not a standard word" in accompanying
comments.

While this Forth can be seen as a toy implementation, I do want it to be usable
for real-world applications.  Forth was originally designed to be something
simple you could build yourself and extend and customize as needed to solve
your problem.  I hope people can use cxxforth like that.

It is assumed that the reader has some familiarity with C++ and Forth.  You may
want to first read the JONESFORTH source or the source of some other Forth
implementation to get the basic gist of how Forth is usually implemented.

[forth]: https://en.wikipedia.org/wiki/Forth_(programming_language) "Forth (programming language)"

[jonesforth]: http://git.annexia.org/?p=jonesforth.git;a=blob;f=jonesforth.S;h=45e6e854a5d2a4c3f26af264dfce56379d401425;hb=HEAD

[markdown]: https://daringfireball.net/projects/markdown/ "Markdown"

[dpans]: http://forth.sourceforge.net/std/dpans/dpansf.htm "Alphabetic list of words"

[forth2012]: http://forth-standard.org/standard/alpha "Forth 2012"

*/


#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <regex>
#include <chrono>

#ifdef FORTHSCRIPTCPP_ENABLE_FILE
// iostream for file operations
#include <fstream>
// smart pointers to store pointers to iostream
#include <memory>
#endif
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT_EXT
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#endif
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT
#include <cmath>
#include <limits>
#endif

#ifndef FORTHSCRIPTCPP_VERSION
#define FORTHSCRIPTCPP_VERSION "1.0.0"
#endif

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

namespace cppforth {

	/****
	 String to convert numeric to string and string to numeric
	****/
	static const std::string digitsCodes{ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
	/****

	The Forth Part
	--------------

	With our C++ kernel defined, now we can define the remainder of the system
	using Forth.  To do this, we will create an array of Forth text lines to be
	evaluated when ForthScriptCPP initializes itself.

	In this section, we won't go into the details of every word defined.  In most
	cases, referring to the standards will be enough to understand what the word is
	supposed to do and the definition will be easy to understand.  But we will
	provide commentary for a few complicated definitions.

	Writing Forth definitions as C++ strings is a little awkward in that we have to
	escape every `"` and backslash with a backslash.

	****/
	static const char*  forthDefinitions =
		/****
		 Exception word set
		 ****/
		 ": catch catchbefore execute catchafter ; "
		 ": abort -1 throw ; "
		 ": evaluate evaluatestart noop evaluatestop ; "
		/****

		I'll start by defining the remaining basic stack operations.  `PICK` and
		`ROLL` are the basis for most of them.

		Note that while I'm not implementing any of the Forth double-cell arithmetic
		operations, double-cell stack operations are still useful.

		****/
		": over    1 pick ; "
		": nip     swap drop ; "
		": tuck    swap over ; "
		": 2over   3 pick 3 pick ; "
		": 2swap   3 roll 3 roll ; "
		": 2>r     swap >r >r ; "
		": 2r>     r> r> swap ; "
		": 2r@     r> r> 2dup >r >r swap ; "


		/****

		`FALSE` and `TRUE` are useful constants.

		****/

		": false   0 ; "
		": true    -1 ; "

		/****

		Forth has a few words for incrementing/decrementing the top-of-stack value.

		****/

		//": 1+ 1 + ; "
		": 1- 1 - ; "

		": cell+   1 cells + ; "
		": char+   1+ ; "
		": chars   ; "

		/****

		`+! ( n|u a-addr -- )` adds a value to a cell in memory.

		****/

		": +!   dup >r @ + r> ! ; "

		/****

		`NEGATE` and `INVERT` can be implemented in terms of other primitives.

		****/

		": negate   0 swap - ; "
		": invert   true xor ; "

		/****

		`, ( x -- )` places a cell value in dataspace.

		`C, ( char -- )` places a character value in dataspace.

		****/
		": ,    here  1 cells allot  ! ; "
		": c,   here  1 chars allot  c! ; "

		/****

		We have a few extended relational operators based upon the kernel's relational
		operators.  In a lower-level Forth system, these might have a one-to-one
		mapping to CPU opcodes, but in this system, they are just abbreviations.

		****/

		": <>    = invert ; "
		": 0<    0 < ; "
		": 0>    0 > ; "
		": 0=    0 = ; "
		": 0<>   0= invert ; "

		/****

		`2*` and `2/` multiply or divide a value by 2 by just shift the bits left or
		right.

		****/

": 2*   1 lshift ; "

/****

A Forth variable is just a named location in dataspace.  We'll use `CREATE` and reserve a cell.

****/
": variable   create 0 , ; "
": ?  @ . ; "


/****

A Forth constant is similar to a variable in that it is a value stored in
dataspace, but using the name automatically puts the value on the stack.  We
can implement this using `CREATE...DOES>`.

****/
": constant    create ,    does>  @ ; "
": 2constant   create , ,  does>  dup cell+ @ swap @ ; "
": 2VARIABLE ALIGN HERE 2 CELLS ALLOT CONSTANT ; "

/****

`/CELL` is not a standard word, but it is useful to be able to get the size
of a cell without using `1 CELLS`.

****/

" 1 cells   constant /cell  "

/****

`DECIMAL` and `HEX` will switch the numeric base to 10 or 16, respectively.

****/

": decimal   10 base ! ; "
": hex       16 base ! ; "

/****

`]` enters compilation mode.

`[` exits compilation mode.

****/

": ]   true state ! ; "
": [   false state ! ; immediate "

/****

`'` gets the next word from the input stream and looks up its execution token.

****/

": '   bl word find drop ; "

/****

The word `LITERAL` takes a cell from the stack at compile time, and at runtime will put that value onto the stack.
We implement this by compiling a call to `(lit)` word followed by the value.

Because we'll be using `(lit)` in other word definitions, we'll create a constant `'(lit)` containing its XT.

****/

"' (lit)     constant '(lit) "
": literal   '(lit) , , ; immediate "
": 2literal swap '(lit) , , '(lit) , , ; immediate "

/****

`[']` is like `'`, but causes the XT to be put on the stack at runtime.

****/

": [']   ' '(lit) , , ; immediate  "

/****

`RECURSE` compiles a call to the word currently being defined.

****/

": recurse     latest , ; immediate  "

": char     bl word char+ c@ ; "
": [char]   char '(lit) , , ; immediate  "
": [DEFINED] BL WORD FIND NIP 0<> ; IMMEDIATE "
": [UNDEFINED] BL WORD FIND NIP 0= ; IMMEDIATE "
/******

 Double-cell numbers

 ******/
 ": MAX 2DUP > IF  ELSE SWAP THEN DROP ; " // ( n1 n2 -- n3 )
 ": MIN 2DUP < IF  ELSE SWAP THEN DROP ; " // ( n1 n2 -- n3 )
 ": SPACE BL EMIT ; " // ( -- ) 
 ": SPACES 0 MAX 0 ?DO SPACE LOOP ; " // ( n -- ) 
 ": MOD /MOD drop ; "
 ": 2@ DUP CELL+ @ SWAP @ ; "
 ": 2! SWAP OVER ! CELL+ ! ; "
 ": DABS 2DUP SWAP DROP 0 < IF DNEGATE THEN ; "
 ": D. DUP >R DABS <# #S R> SIGN #> TYPE SPACE ; " // ( d -- )
 ": U. 0 D. ; " // ( u-- )
 ": U.R  >R 0 <# #S #> R> OVER - 0 MAX SPACES TYPE ; " // ( u n -- )
 ": D.R >R <# #S #>  R> OVER - 0 MAX SPACES TYPE ; " // ( d n -- )
 " : MOVE ROT ROT 2DUP < IF ROT CMOVE> ELSE ROT CMOVE THEN ; "
 " : WITHIN OVER - >R - R> U< ;  "
 //" : MARKER CREATE ; immediate  " //@bug - no implemented
 " : BUFFER:  CREATE ALLOT ;  " // (u \"<name>\" --; --addr)
		/****

		Here are some more words we can define now that we have control structures.

		****/
		
		": ?dup       dup 0<> if dup then ; "

		": abs        dup 0 < if negate then ; "

		": max        2dup < if nip else drop then ; "
		": min        2dup < if drop else nip then ; "

		": space      bl emit ; "
		": spaces     begin  dup 0> while  space 1-  repeat  drop ; " 

		/****

		I wish I could explain `POSTPONE`, but I can't, so you will just have to google it.

		POSTPONE discussion: https://github.com/ForthHub/discussion/discussions/105
		****/

		": postpone bl word find  1 = if , else  '(lit) , ,  ['] , ,  then ; immediate "

/****

`DEFER` and `IS` are similar to `VALUE` and `TO`, except that the value is an
execution token, and when the created word is used it invokes that xt.  `IS`
can be used to change the execution token.  In C++ terms, you can think of this
as a pointer to a function pointer.

`DEFER` and `IS` are not ANS Forth standard words, but are in common use, and
are described formally in a proposal at
<http://forth-standard.org/standard/core/DEFER>.

****/

": defer       create ['] abort ,  does> @ execute ; "
": defer@      >body @ ; "
": defer!      >body ! ; "
": is          state @ if "
"                  postpone ['] postpone defer! "
"              else "
"                  ' defer! "
"              then ; immediate "

": action-of   state @ if "
"                  postpone ['] postpone defer@ "
"              else "
"                  ' defer@ "
"              then ; immediate "

/****

Strings
-------

`S" ( "ccc<quote>" -- caddr u )`

This word parses input until it finds a `"` (double quote) and then puts the
resulting string's address and length on the stack.  It works in both
compilation and interpretation mode.

In interpretation mode, it just returns the address and length of the std::string in
the input buffer.

In compilation mode, we have to copy the string somewhere where it can be found
at execution time.  The word `SLITERAL` implements this.  It compiles a
forward-branch instruction, then copies the string's characters into the
current definition between the branch and its target instruction, then at the
branch target location we use a couple of `LITERAL`s to put the address and
length of the word in the definition onto the stack.

`." ( "ccc<quote>" -- )`

This word prints the given string.  We can implement it in terms of `S"` and
`TYPE`.

`.( ( "ccc<quote>" -- )`

This is like `."`, but is an immediate word.  It can be used to display output
during the compilation of words.

****/
": thenthenthen dup  here swap -  swap ! ; immediate  " // used in 'sliteral' word definition

": sliteral "
"    >r "
"    ['] (branch) , here >r 0 , "  // compile a branch with dummy offset
"    r> r> 2dup >r >r "
"    swap cell+ swap "             // copy into the first byte after the offset
"    dup allot  cmove align "      // allocate dataspace and copy string into it
"    r> dup postpone thenthenthen  " // resolve the branch 
"    cell+ postpone literal "      // compile literal for address
"    r> postpone literal "         // compile literal for length
" ; immediate  "

" : s\" [char] \" parse state @ if postpone sliteral then ; immediate "

" : .\" postpone s\" postpone type ; immediate "

" : .(  [char] ) parse type ; immediate "
" : ERASE 0 FILL ;  "

/****

`/STRING ( c-addr1 u1 n1 -- c-addr2 u2 )` adjusts a character string by adding
to the address and subtracting from the length.

****/

": /string   dup >r - swap r> + swap ; "

/****

`ABORT"` checks whether a result is non-zero, and if so, it throws an exception
that will be caught by `QUIT`, which will print the given message and then
continue the interpreter loop.

****/

//": (abort\")   rot if abort-message then 2drop ; "
": (abort\")   rot if type -1 throw then 2drop ; "
": abort\"     postpone s\" postpone (abort\") ; immediate "

#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT
": fover    1 fpick ; "
": f, here  1 floats allot  f! ; " // not standard word
" ' (flit) constant '(flit)  "
": fliteral   '(flit) , f, ; immediate  "
": fconstant    create f,    does>  f@ ; "
": fvariable   create 0.0e0 f, ; "
": F> FSWAP F< ; " // not standard word
": SFLOATS FLOATS ; : SFLOAT+ FLOAT+ ; : DFLOATS FLOATS ; : DFLOAT+ FLOAT+ ;  "
#endif

		/****
		A Forth `VALUE` is just like a constant in that it puts a value on the stack
		when invoked.  However, the stored value can be modified with `TO`.

		`VALUE` is implemented as 2 part. 
		 First part is WORD, that contains VALUE type:
		 1 - WORD
		 2 - DOUBLE
		 3 - FLOAT
		SECOND part length depends on VALUE type.
		 for 1 - 1 WORD
		 for 2 - 1 DOUBLE word (2 WORDs)
		 for 3 - 1 FLOAT
		****/

//": value constant ; "
//": value! >body ! ; "
//": 2value! >body 2! ; "
//": 2VALUE CREATE , , DOES> 2@ ; "
//": VALUE CREATE ,  DOES> @ ; "
//": to       state @ if postpone ['] postpone value! else ' value! then ; immediate "
//": TO ' >BODY STATE @ IF POSTPONE 2LITERAL POSTPONE 2! ELSE 2! THEN ; IMMEDIATE "
//": to       state @ if postpone ['] postpone 2value! else ' 2value! then ; immediate "
": value create 1 , , does> dup @ 1 = if cell+ @ else then ; "
": 2value create 2 , , , does> dup @ 2 = if cell+ 2@ else then ; " 
": fvalue create 3 , F,  does> dup @ 3 = if cell+ F@ else then ; "
" : to state @ if 3 2 1 then ' >body state @ if "
"postpone literal postpone dup postpone @ postpone dup postpone literal postpone = "
"postpone if postpone drop postpone cell+ postpone ! postpone else postpone dup postpone literal postpone = "
"postpone if postpone drop postpone cell+ postpone 2! postpone else postpone literal postpone = "
"postpone if postpone cell+ postpone F! postpone then postpone then postpone then "
"else dup @ dup 1 = if drop cell+ ! else dup 2 = if drop cell+ 2! else 3 = if cell+ F! then then then then ; immediate "

#ifdef FORTHSCRIPTCPP_ENABLE_FILE
/****

`INCLUDED` is the word for reading additional source files. For example, you
can include the file `tests/hello.fs` and then run its `hello` word by doing
the following:

s" tests/hello.fs" INCLUDED
hello

`INCLUDE` is a simpler variation, used like this:

INCLUDE tests/hello.fs
hello

`INCLUDE` is not part of the AND standard, but is in Forth 2012.

****/

": included "
"    r/o open-file abort\" included: unable to open file\" "
"    dup include-file "
"    close-file abort\" included: unable to close file\" ; "

": include   bl word count included ; "

#endif // #ifdef FORTHSCRIPTCPP_ENABLE_FILE

/****

Comments
--------

There is a good reason that none of our Forth defintions above have had any
stack diagrams or other comments: our Forth doesn't support comments yet.  We
have to define words to implement comments.

We will support two standard kinds of Forth comments:

- If `\` (backslash) appears on a line, the rest of the line is ignored.
- Text between `(` and `)` is ignored.

Also, we will allow `#!` as a synonym for `\`, so that we can start a
UNIX shell script with something like this:

#! /usr/local/bin/ForthScriptCPP

Note that a space is required after the `\`, `(`, or `#!` that starts a
comment.  They are blank-delimited words just like every other Forth word.

****/

": \\   source nip >in ! ; immediate "
": #!   postpone \\ ; immediate "
": (    [char] ) parse 2drop ; immediate "
/****
S\" http://www.forth200x.org/escaped-strings.html

****/
" decimal : c+! tuck c@ + swap c! ;  "
" : addchar   tuck count + c!  1 swap c+! ; "
" : append  >r tuck  r@ count +  swap cmove   r> c+! ; "
" : extract2H base @ >r  hex 0 0 2over drop 2 >number 2drop drop  "
"    >r 2 /string  r>  r> base ! ;  "
" create EscapeTable  7 c, 8 c, char c c, char d c, 27 c, 12 c, char g c, char h c, char i c,  "
" char j c, char k c, 10 c, char m c, 10 c, char o c, char p c, char \" c, 13 c, char s c, 9 c,  "
" char u c, 11 c, char w c, char x c, char y c, 0 c,  "
" create CRLF$ 2 c,  13 c,  10 c,  "
" : addEscape over 0= if  drop  exit  then >r over c@ [char] x = if  "
"    1 /string extract2H r> addchar  exit then over c@ [char] m = if  "
"    1 /string  13 r@ addchar  10 r> addchar  exit  then over c@ [char] n = if  "
"    1 /string  crlf$ count r> append  exit then over c@ [char] a [char] z 1+ within if  "
"    over c@ [char] a - EscapeTable + c@  r> addchar else over c@ r> addchar then 1 /string ;  "
" : parse\" dup >r  0 swap c! begin dup while over c@ [char] \" <> while over c@ [char] \\ = if  "
"  1 /string r@ addEscape else over c@ r@ addchar  1 /string then repeat then dup   "
"  if 1 /string  then  r> drop ;  "
" create pocket  256 1 chars + allot  "
" : readEscaped source >in @ /string tuck pocket parse\" nip - >in +! pocket ;  "
" : S\\\" readEscaped count  state @  if  postpone sliteral  then ; IMMEDIATE  "

/******

CASE implementation https://forth-standard.org/standard/rationale#rat:core:SOURCE

*****/
" 0 CONSTANT CASE IMMEDIATE  "
" : OF 1+ >R POSTPONE OVER POSTPONE = POSTPONE IF POSTPONE DROP R> ; IMMEDIATE  "
" : ENDOF >R   POSTPONE ELSE R> ; IMMEDIATE  "
" : ENDCASE POSTPONE DROP   0 ?DO POSTPONE THEN LOOP ; IMMEDIATE  "
/******
 [IF] [ELSE] [THEN] implementation
 ******/
 " : [THEN] ( -- ) ; IMMEDIATE  "
 " : [ELSE] 1 BEGIN " // level 
 "       BEGIN BL WORD COUNT DUP WHILE " //              level adr len 
 "       2DUP S\" [IF]\" COMPARE 0= IF "	//              level adr len 
 "             2DROP 1+ " //                             level' 
 "          ELSE " //                                   level adr len 
 "            2DUP S\" [ELSE]\" COMPARE 0= IF " //	    level adr len 
 "                2DROP 1- DUP IF 1+ THEN "  //          level' 
 " ELSE " //	                                level adr len 
 " S\" [THEN]\" COMPARE 0= IF " //        level 
 "                   1- " //                             level' 
 "               THEN " //
 "             THEN " //
 "           THEN ?DUP 0= IF EXIT THEN " //              level' 
 "       REPEAT 2DROP " //                              	level 
 "   REFILL 0= UNTIL " //                                level 
 "    DROP ; IMMEDIATE  "
 " : [IF] ( flag -- ) 0= IF POSTPONE [ELSE] THEN ; IMMEDIATE  "
 " : [UNDEFINED] BL WORD FIND NIP 0= ; IMMEDIATE  "
/**
 * Core-ext 
 * 
*/
": isspace? ( c -- f ) BL 1+ U< ; "
": isnotspace? ( c -- f ) isspace? 0= ; "
": xt-skip " //( addr1 n1 xt -- addr2 n2 ) \ skip all characters satisfying xt ( c -- f ) 
"   >R BEGIN DUP WHILE OVER C@ R@ EXECUTE WHILE 1 /STRING REPEAT THEN R> DROP ; "
": parse-name " // ( "name" -- c-addr u ) 
"SOURCE >IN @ /STRING ['] isspace? xt-skip OVER >R ['] isnotspace? xt-skip " // ( end-word restlen r: start-word ) 
"2DUP 1 MIN + SOURCE DROP - >IN ! DROP R> TUCK - ; "

 /** 
  *  Facility implementation
  * 
 */

": SFALIGNED ALIGNED ; "
": DFALIGNED ALIGNED ; "
": BEGIN-STRUCTURE CREATE HERE 0 0 , DOES> @ ; "
": END-STRUCTURE SWAP ! ; "
": +FIELD CREATE OVER , + DOES> @ + ; "
": FIELD:    ( n1 \"name\" -- n2 ; addr1 -- addr2 )  ALIGNED 1 CELLS +FIELD ; "
": CFIELD:   ( n1 \"name\" -- n2 ; addr1 -- addr2 )  1 CHARS   +FIELD ; "
": FFIELD:   ( n1 \"name\" -- n2 ; addr1 -- addr2 )  FALIGNED 1 FLOATS +FIELD ; "
": SFFIELD:  ( n1 \"name\" -- n2 ; addr1 -- addr2 )  SFALIGNED 1 SFLOATS +FIELD ; "
": DFFIELD:  ( n1 \"name\" -- n2 ; addr1 -- addr2 )  DFALIGNED 1 DFLOATS +FIELD ; "
/**
 *  Locals
 * 
*/
/*
"12345 CONSTANT undefined-value "
": match-or-end? ( c-addr1 u1 c-addr2 u2 -- f ) 2 PICK 0= >R COMPARE 0= R> OR ; "
": scan-args " // 0 c-addr1 u1 -- c-addr1 u1 ... c-addrn un n c-addrn+1 un+1
"  BEGIN "
"2DUP S\" |\" match-or-end? 0= WHILE "
"  2DUP S\" --\" match-or-end? 0= WHILE "
"  2DUP S\" :}\" match-or-end? 0= WHILE "
"  ROT 1+ PARSE-NAME "
"AGAIN THEN THEN THEN ; "
": scan-locals " // n c-addr1 u1 -- c-addr1 u1 ... c-addrn un n c-addrn+1 un+1 
"  2DUP S" |" COMPARE 0= 0= IF EXIT THEN "
"   2DROP PARSE-NAME "
"   BEGIN "
"   2DUP S\" --\" match-or-end? 0= WHILE "
"     2DUP S\" :}\" match-or-end? 0= WHILE " 
"     ROT 1+ PARSE-NAME "
"     POSTPONE undefined-value "
"   AGAIN THEN THEN ; "
": scan-end ( c-addr1 u1 -- c-addr2 u2 ) "
"  BEGIN "" 
"     2DUP S\" :}\" match-or-end? 0= WHILE "
"     2DROP PARSE-NAME "
"   REPEAT ; "
": define-locals ( c-addr1 u1 ... c-addrn un n -- ) 0 ?DO (LOCAL) LOOP 0 0 (LOCAL) ; "
": {: ( -- ) 0 PARSE-NAME scan-args scan-locals scan-end 2DROP define-locals ; IMMEDIATE "
*/
/** 
 *  Strings
*/
// http://lars.nocrew.org/forth2012/xchar/X-SIZE.html 
": XCHAR- ( xc-addr -- xc-addr' ) BEGIN 1 CHARS - DUP C@ $C0 AND $80 <> UNTIL ; "
": X-SIZE ( xc-addr u1 -- u2 ) 0= IF DROP 0 EXIT THEN " // \ length of UTF-8 char starting at u8-addr (accesses only u8-addr) 
" C@ "
"  DUP $80 U< IF DROP 1 EXIT THEN "
"  DUP $c0 U< IF -77 THROW THEN "
"  DUP $e0 U< IF DROP 2 EXIT THEN "
"  DUP $f0 U< IF DROP 3 EXIT THEN "
"  DUP $f8 U< IF DROP 4 EXIT THEN "
"  DUP $fc U< IF DROP 5 EXIT THEN "
" DUP $fe U< IF DROP 6 EXIT THEN "
"  -77 THROW ; "
": -TRAILING-GARBAGE ( xc-addr u1 -- xc-addr u2 )  2DUP + DUP XCHAR- ( addr u1 end1 end2 ) 2DUP DUP OVER OVER - X-SIZE + " 
" = IF 2DROP ELSE NIP NIP OVER - THEN ; "
": BLANK ( c-addr u -- ) BL FILL ; "
/***
 * Search
*/
/***
 * Strings EXT
 * Substitute REPLACE UNESCAPE
*/
": UNESCAPE " // \ c-addr1 len1 c-addr2 -- c-addr2 len2 
 //\ Replace each '%' character in the input string c-addr1 len1 with two '%' characters. 
// \ The output is represented by c-addr2 len2. 
// \ If you pass a string through UNESCAPE and then SUBSTITUTE, you get the original string. 
" DUP 2SWAP OVER + SWAP ?DO I C@ [CHAR] % = IF [CHAR] % OVER C! 1+ THEN I C@ OVER C! 1+ LOOP OVER - ; "
/***
 * TOOLS
*/

/****

`ABOUT` is not a standard word.  It just prints licensing and credit information.

`.DQUOT` is also not a standard word.  It prints a double-quote (") character.

****/
		": .dquot   [char] \" emit ; " 

		": about "
		"      cr "
		"      .\" For more, visit <https://github.com/VladlenPopolitov/ForthScriptCpp>.\" cr ; "


		": welcome "
		"    .\" ForthScriptCPP " FORTHSCRIPTCPP_VERSION "\" cr "
		"    .\" Type \" .dquot .\" about\" .dquot .\"  for more information.  \" "
		"    .\" Type \" .dquot .\" bye\" .dquot .\"  to exit.\" cr ; "
	;


	/****

	That is the end of our built-in Forth definitions.

	With the `forthDefinitions`  filled, all we need to do is call
	`INTERPRET` to load definitions into the system.

	****/


	class Forth {
		/****

		----

		Data Types
		----------

		Next I define some types.

		A `Cell` is the basic Forth type.  I define the `Cell` type using the C++
		`uintptr_t` type to ensure it is large enough to hold an address.  This
		generally means that it will be a 32-bit value on 32-bit platforms and a 64-bit
		value on 64-bit platforms.  (If you want to build a 32-bit Forth on a 64-bit
		platform with clang or gcc, you can pass the `-m32` flag to the compiler and
		linker.)

		Forth doesn't require type declarations; a cell can be used as an address, an
		unsigned integer, a signed integer, or a variety of other uses.  However, in
		C++ we will have to be explicit about types to perform the operations we want
		to perform.  So I define a few additional types to represent the ways that a
		`Cell` can be used, and a few macros to cast between types without littering
		the code with a lot of `reinterpret_cast<...>(...)` expressions.

		****/

	public:
		typedef unsigned int Cell;      // unsigned value
		typedef signed int  SCell;     // signed value
	private:
		class DCell {
		public:
			union {
				struct {
					Cell lo, hi;
				} Cells;
				struct {
					SCell lo, hi;
				} SCells;
				unsigned long long int Dcells;
				signed long long int SDcells;
			} data_;
			DCell()  { data_.Cells.lo = 0; data_.Cells.hi = 0; };
			DCell(Cell valueLo,Cell valueHi) { data_.Cells.lo = valueLo; data_.Cells.hi = valueHi; };
			DCell(DCell &value)  {
				data_.Cells.lo = value.data_.Cells.lo; data_.Cells.hi = value.data_.Cells.hi;
			};
			DCell(unsigned long long int value)  {
				data_.Dcells = value;
			}
		};
		class SDCell {
		public:
			union {
				struct {
					SCell lo, hi;
				} Cells;
				 long long int Dcells;
			} data_;
			SDCell()  { data_.Cells.lo = 0; data_.Cells.hi = 0; };
			SDCell(SCell valueLo, SCell valueHi) { data_.Cells.lo = valueLo; data_.Cells.hi = valueHi; };
			SDCell(SDCell &value)  {
				data_.Cells.lo = value.data_.Cells.lo; data_.Cells.hi = value.data_.Cells.hi;
			};
			SDCell(long long int value)  {
				data_.Dcells = value;
			}
		};
		typedef unsigned char Char;
		typedef signed char SChar;

		typedef Cell CAddr;          // Any address
		typedef Cell AAddr;          // Cell-aligned address

#define CELL(x)    (Cell)(x)
#define CADDR(x)   static_cast<CAddr>(x)
#define AADDR(x)   static_cast<AAddr>(x)
#define CHARPTR(x) reinterpret_cast<char*>(x)
#define SIZE_T(x)  static_cast<std::size_t>(x)

		const size_t CellSize = sizeof(Cell);
		const size_t DCellSize = sizeof(DCell);

		/****

		Boolean Constants
		-----------------

		Here I define constants for Forth _true_ and _false_ Boolean flag values.

		Note that the Forth convention is that a true flag is a cell with all bits set,
		unlike the C++ convention of using 1 or any other non-zero value to mean true,
		so we need to be sure to use these constants for all Forth words that return a
		Boolean flag.

		****/

		const Cell False = 0;
		const Cell True = ~False;
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
		std::stringstream std_cout{},  std_cerr{};
#endif
		std::stringstream  std_cin{};
		/****

		Configuration Constants
		-----------------------

		I have a few macros to define the size of the Forth data space, the maximum
		numbers of cells on the data and return stacks
		****/

		const size_t FORTHSCRIPTCPP_DATASPACE_SIZE = (1 * 1024 * 1024 * sizeof(Cell));
		const size_t FORTHSCRIPTCPP_DSTACK_COUNT = 1556;
		const size_t FORTHSCRIPTCPP_RSTACK_COUNT = 1556;

		/****

		----

		The Definition Struct
		---------------------

		My first big departure from traditional Forth implementations is how I will
		store the word definitions for the Forth dictionary.  Traditional Forths
		intersperse the word names in the shared data space along with code and data,
		using a linked list to navigate through them.  I am going to use a `std::vector`
		of `Definition` structs, outside of the data space.

		Use of `std::vector` has these benefits:

		- The `Definition` structures won't use data space.  The C++ library will take care of allocating heap space as needed.

		One of the members of `Definition` is a C++ `std::string` to hold the name.  I
		won't need to worry about managing the memory for that variable-length field.
		The `name` field will be empty for a `:NONAME` definition.

		A `Definition` also has a `code` field that points to the native code
		associated with the word, a `does` field pointing to associated Forth
		instructions, a `parameter` field that points to associated data-space
		elements, and some bit flags to keep track of whether the word is `IMMEDIATE`
		and/or `HIDDEN`.  We will explore the use of these fields later when I talk
		about the inner and outer interpreters.

		`Definition` has a static field `executingWord` that contains the address
		of the `Definition` that was most recently executed.  This can be used by
		`Code` functions to refer to their definitions.

		Finally, `Definition` has a few member functions for executing the code and for
		accessing the _hidden_ and _immediate_ flags.

		****/

		typedef void(Forth::*Code)(void);

		struct Definition {

			Code   code;
			Cell  does;
			AAddr  parameter;
			Cell   flags;
			std::string name;
			Cell numberInVector;
			const Cell FlagHidden = (1 << 1);
			const  Cell FlagImmediate = (1 << 2);

			Definition() : code{ nullptr }, does{ 0 }, parameter{ 0 }, flags{ 0 },
				name{}, numberInVector{}
			{
			}

			// Example, how to call execute():
			//	auto mainXt = getDefinition(findDefinition("MAIN"));
			//	if (!mainXt)
			//		throw std::runtime_error("MAIN not defined");
			//	mainXt->executeDefinition(*this);
			void executeDefinition(Forth &pointer) const {
				auto saved = pointer.executingWord;
				pointer.executingWord = numberInVector;
				//std::cout << numberInVector << " " << name << " " << code << std::endl;
				if (code) {
					if(pointer.isTraceCalls()) std::cout << name << " ";
					CALL_MEMBER_FN(pointer, code)();
				}
				else {
					throw AbortException("Word \""+name+"\" has nullptr code");
				}
				pointer.executingWord = saved;
			}

			bool isHidden() const    { return (flags & FlagHidden) != 0; }

			void toggleHidden()      { flags ^= FlagHidden; }

			bool isImmediate() const { 
				return (flags & FlagImmediate) != 0;
			}

			void toggleImmediate()   { 
				flags |= FlagImmediate;  
			}

			bool isFindable() const { return !name.empty() && !isHidden(); }
		};


		/****

		I will use the index of the `Definition` in std::vestor as the Forth _execution token_ (XT).

		****/

		typedef Cell Xt;
#define XT(x) static_cast<Xt>(x)

		/****

		I have to define the `executingWord` member used in `Definition`.

		****/
	private:
		Xt executingWord = 0;

		// Split std::string by delimiters in tokens
		void split_str(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
		{
			std::string::size_type delimitersSize = delimiters.size();
			std::string::size_type pos = str.find(delimiters, 0);
			std::string::size_type lastPos = str.find(delimiters, pos + delimitersSize);
			tokens.push_back(str.substr(0, pos));

			while (str.find_first_not_of(delimiters, pos) != std::wstring::npos)
			{
				tokens.push_back(str.substr(pos + delimitersSize, lastPos - pos - delimitersSize));
				pos = str.find(delimiters, lastPos);
				lastPos = str.find(delimiters, pos + delimitersSize);
			}
		}
#ifdef _DEBUG
		/****

		method for trace and debug of this class. You can place breakpoints inside and view stacks content

		****/
		bool trace = false;
		void traceDebug(const std::string &name){
			if (trace){
				int here = getDataPointer();
				std::vector<Cell> stack{ dStack.debugStack() };
				std::vector<Cell> stackIf{ controlStackIf_Begin.debugStack() };
				std::vector<Cell> stackLoop{ controlStackLoops.debugStack() };
				std::vector<Cell> stackr{ rStack.debugStack() };
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT
				std::vector<FCell> stackf{ fStack.debugStack() };
#endif
				std::cout << "Execute:" << name << std::endl;
			}
		}

	public:
		void setTrace(bool value){
			trace = value;
		}
	private:
#endif
		/****

		----

		Global Variables
		----------------

		With the types defined, next I define global variables, starting with the Forth
		data space and the data and return stacks.

		Each of these arrays is implemented as std::vector<Cell>, 
		so I can easily test with size() method whether I need to report an overflow.

		****/

		template <typename CellType> class ForthStack {
			std::vector<CellType> stack;
			size_t top;
		public:
			ForthStack() : top{ 0 }, stack{}{
				stack.resize(256); 
			};
			void resize(size_t newsize){
				stack.resize(newsize);
			};
			CellType getTop(){
				if ( (top) >= stack.size()){
					throw AbortException("Forth stack overflow");
				}
				return stack.at(top);
			}
			// returns *(top-index)
			CellType getTop(size_t index){
				{auto index1 = top - index;
				if ((index1) < 0 || (index1) >= stack.size()){
					throw AbortException("Forth stack overflow");
				}
				}
				return stack.at(top - index);
			}
			void setTop(CellType Value){
				{auto index1 = top ;
				if ((index1) < 0 || (index1) >= stack.size()){
					throw AbortException("Forth stack overflow");
				}
				}
				stack.at(top) = Value;
			}
			// sets *(top-index) = Value
			void setTop(size_t index, CellType Value){

				{auto index1 = top - index;
				if ((index1) < 0 || (index1) >= stack.size()){
					throw AbortException("Forth stack overflow");
				}
				}
				stack.at(top - index) = Value;
			}
			void moveStack(size_t to, size_t from, size_t count){
				auto stackSize = stack.size();
				if (count >= 0 && (top - to) >= 0 && (top - to) < stackSize &&
					(top - to + count) < stackSize &&
					(top - from) >= 0 && (top - from) < stackSize &&
					(top - from + count) < stackSize
					){
					std::memmove(&stack[top - to], &stack[top - from], count * sizeof(CellType));
				}
				else {
					throw AbortException(std::string("access of data stack outside of borders"));
				}
			}
			bool availableStack(size_t n) {
				return	(top + n) < stack.size();
			}
			void push(CellType x) {
				{auto index1 = top + 1;
				if ((index1) < 0 || (index1) >= stack.size()){
					throw AbortException("Forth stack overflow");
				}
				}
				stack.at(++top) = x;
			}
			void pop() {
				if (top == 0){
					throw AbortException("Forth stack underflow");
				}
				--top;
			}
			void resetStack() {
				top = 0;
				std::memset(&stack[0], 0, stack.size()*sizeof(stack[0]));

			}
			// Return the depth of the data stack.
			std::size_t stackDepth() {
				return top ;
			}
			// return stack content as std::vector for debug and trace 
			std::vector<CellType> debugStack(){
				std::vector<CellType> stackForReturn = stack;
				stackForReturn.resize(top + 1);
				return stackForReturn;
			}
		};
		// Data stack
		ForthStack<Cell> dStack{}; 
		// Return stack R> >R
		ForthStack<Cell> rStack{};
		// control stack for DO LOOP
		ForthStack<Cell> controlStackLoops{};
		// control stack for IF ELSE THEN Begin While Repeat Until Again
		ForthStack<Cell> controlStackIf_Begin{};

		//std::vector<Char> dataSpace{};
	
		/****

		The Forth dictionary is a vector of `Definition`s.  The most recent definition is
		at the back of the list.

		****/

		std::vector<Definition> definitions{};

		Definition *getDefinition(Cell index){
			if ((index < 0) || (index >= definitions.size())){
					throwMessage("Access to definitions out of range",errorUndefinedWord);
			}
			return &definitions[index];
		}

		/****

		For each of the global arrays, I need a pointer to the current location.

		For the data space, we have the `getDataPointer`, which corresponds to Forth's
		`HERE`.

		For each of the stacks, we need a pointer to the element at the top of the
		stack.  The stacks grow upward.  When a stack is empty, the associated pointer
		points to an address below the actual bottom of the array, so we will need to
		avoid dereferencing these pointers under those circumstances.

		****/

		
			void setIsCompiling(Cell value){
				setDataCell(getIsCompilingAddress(), value);
			}

			Cell getIsCompiling() { 
				auto value=getDataCell(getIsCompilingAddress());
			    return value;
			}

			CAddr getIsCompilingAddress()  { return VarOffsetState; }

		size_t dataPointer = 0;
		size_t getDataPointer(){
			return dataPointer;
		}
		void setDataPointer(size_t value){
			dataPointer = value;
		}
		void incDataPointer(size_t value){
			setDataPointer(getDataPointer() + value);
		}
		const CAddr VarOffsetState = CellSize*1;
		const CAddr VarOffsetSourceAddress = CellSize * 2;  // SOURCE variable
		const CAddr VarOffsetSourceSize = CellSize * 3;		// SOURCE buffer size
		const CAddr VarOffsetSourceOffset = CellSize * 4;   // >IN variable
		const CAddr VarOffsetBlkAddress = CellSize * 5;		// BLK variable
		const CAddr VarOffsetNumericBase = CellSize * 6;	// BASE variable
		const CAddr VarOffsetWordBuffer = CellSize * 7;
		const CAddr WordBufferSize = CellSize * 256;
		const CAddr PadBufferSize = 256;
		const CAddr VarOffsetPadBuffer = WordBufferSize + CellSize * 8;
		const CAddr VarOffsetSourceBuffer = WordBufferSize + CellSize * 8 + PadBufferSize;


		Char getDataChar(CAddr pointer){
			return dataSpaceAt(pointer);
		}
		void setDataChar(CAddr pointer, Char value){
			dataSpaceSet(pointer, value);
		}
		Cell getDataCell(CAddr pointer){
			Cell tmp{ 0 }, tmpRes{ 0 };
			for (size_t i = 0; i < sizeof(Cell); ++i){
				tmp=dataSpaceAt(pointer + i) ;
				tmp <<= 8*i;
				tmpRes |= tmp;
			}
			return tmpRes;
		}
		void setDataCell(AAddr pointer, Cell value){
			Cell tmp{ 0 };
			for (size_t i = 0; i < sizeof(Cell); ++i){
				dataSpaceSet(pointer+i, value & 0x00ff);
				value >>= 8;
			}
		}
		 
		Definition &definitionsAt(Cell index){
			if (index >= definitions.size() || index<0){
				throwMessage("Access to definitions out of range",errorUndefinedWord);
			}
			return definitions.at(index);
		}

		/****

		The inner-definition interpreter needs a pointer to the next instruction to be
		executed.  This will be explained below in the **Inner Interpreter** section.

		****/
		Xt next_command = 0;

		/****

		There are a few special words whose XTs I will use frequently when compiling
		or executing.  Rather than looking them up in the dictionary as needed, I'll
		cache their values during initialization.

		****/


		/****

		I need a flag to track whether we are in interpreting or compiling state.
		This corresponds to Forth's `STATE` variable.

		****/


		/****

		I provide a variable that controls the numeric base used for conversion
		between numbers and text.  This corresponds to the Forth `BASE` variable.

		Whenever using C++ stream output operators, I will need to ensure the stream's
		numeric output base matches `numericBase`.  To make this easy, I'll define a
		macro `SETBASE()` that calls the `std::setbase` I/O manipulator and use it
		whenever writing numeric data using the stream operators.

		****/

#define SETBASE() std::setbase(static_cast<int>(getNumericBase())) 

		/****

		The input buffer is a `std::vector<Char>`.  This makes it easy to use the C++ I/O
		facilities, and frees me from the need to allocate a statically sized buffer
		that could overflow.  I also have a current offset into this buffer,
		corresponding to the Forth `>IN` variable.

		****/


		void setSourceAddress(Cell value) {
			setDataCell(VarOffsetSourceAddress, value);
		}
		Cell getSourceAddress() {
			return getDataCell(VarOffsetSourceAddress);
		}

		void setSourceBufferSize(Cell value){
			setDataCell(VarOffsetSourceSize, value);
		}
		Cell getSourceBufferSize() {
			return getDataCell(VarOffsetSourceSize);
		}
		void setSourceBuffer(const std::string &value){
			auto &sourceBufferVirtual = VirtualMemory.at(vmSegmentSourceBufferRefill).segment;
			sourceBufferVirtual.resize(0);
			std::copy(value.begin(), value.end(), std::back_inserter(sourceBufferVirtual));
			VirtualMemory.at(vmSegmentSourceBufferRefill).end = VirtualMemory.at(vmSegmentSourceBufferRefill).start + VirtualMemory.at(vmSegmentSourceBufferRefill).segment.size();
			setSourceVariables(VirtualMemory.at(vmSegmentSourceBufferRefill).start, sourceBufferVirtual.size(), 0);
			//setSourceBufferSize(sourceBufferVirtual.size());
		}
		bool setSourceBuffer(){
			if (inputBufferStringsCurrent < inputBufferStrings.size()){
				// load next string from input buffer
				const std::string &value = inputBufferStrings.at(inputBufferStringsCurrent);
				auto &sourceBufferVirtual = VirtualMemory.at(vmSegmentSourceBufferRefill).segment;
				sourceBufferVirtual.resize(0);
				std::copy(value.begin(), value.end(), std::back_inserter(sourceBufferVirtual));
				VirtualMemory.at(vmSegmentSourceBufferRefill).end = VirtualMemory.at(vmSegmentSourceBufferRefill).start + VirtualMemory.at(vmSegmentSourceBufferRefill).segment.size();
				setSourceVariables(VirtualMemory.at(vmSegmentSourceBufferRefill).start, sourceBufferVirtual.size(), 0);
				//setSourceBufferSize(sourceBufferVirtual.size());
				//setSourceBufferOffset(0);
				return true;
			}
			else {
				// fill input buffer by zero length vector
				setSourceVariables(VirtualMemory.at(vmSegmentSourceBufferRefill).start, 0, 0);
				//setSourceBufferSize(0);
				//setSourceBufferOffset(0);
				return false;
			}
		}
		/// <summary>
		/// Set information bout source buffer
		/// </summary>
		/// <param name="Source">SOURCE buffer address</param>
		/// <param name="sourceSize">SOURCE size</param>
		/// <param name="sourceMoreIn"> >IN variable value </param>
		void setSourceVariables(Cell Source, Cell sourceSize, Cell sourceMoreIn) {
			setSourceAddress(Source);
			setSourceBufferSize(sourceSize);
			setSourceBufferOffset(sourceMoreIn);
		}
		void getSourceVariables(Cell &Source, Cell &sourceSize, Cell &sourceMoreIn) {
			Source=getSourceAddress();
			sourceSize=getSourceBufferSize();
			sourceMoreIn=getSourceBufferOffset();
		}
		/// <summary>
		///  set Source Buffer offset
		/// </summary>
		/// <param name="value"> Value if >IN variable</param>
		void setSourceBufferOffset(Cell value){
			setDataCell(VarOffsetSourceOffset, value);
		}
		Cell getSourceBufferOffset(){
			return getDataCell(VarOffsetSourceOffset);
		}
		void incSourceBufferOffset(){
			setSourceBufferOffset(getSourceBufferOffset() + 1);
		}
		int getSourceBufferRemain() {
			return getSourceBufferSize() - getSourceBufferOffset();
		}
		void setNumericBase(Cell value){
			setDataCell(VarOffsetNumericBase, value);
		}
		Cell getNumericBase(){
			return getDataCell(VarOffsetNumericBase);
		}

		/****

		the pictured numeric output buffer is a `std::vector<Char>`.  This makes it easy to use the C++ I/O
		facilities, and frees me from the need to allocate a statically sized buffer
		that could overflow. 

		****/
		std::vector<Char> picturedInputBuffer;
		
		/****

		----

		Stack Primitives
		----------------

		I will be doing a lot of pushing and popping values to and from our data and
		return stacks, so in lieu of sprinkling pointer arithmetic throughout our code,
		I'll define a few simple functions to handle those operations.  I expect the
		compiler to expand calls to these functions inline, so this isn't inefficient.

		Above I defined the global variables `dTop` and `rTop` to point to the top of
		the data stack and return stack.  I will use the expressions `getDTop()` and
		`getRTop()` when accessing the top-of-stack values.  I will also use expressions
		like `getDTop( 1 )` and `getDTop( 2 )` to reference the items beneath the top of
		stack.

		When I need to both read and remove a top-of-stack value, my convention will
		be to put both operations on the same line, like this:

		Cell x = getDtop(); pop();

		A more idiomatic C++ way to write this might be `Cell x = *(dTop--);`, but I
		think that's less clear.

		****/



		// Push cell onto data stack.
		void push(Cell x) {
			dStack.push( x );
		}

		// Pop cell from data stack.
		void pop() {
			dStack.pop();
		}

		// Push cell onto return stack.
		void rpush(Cell x) {
			rStack.push( x );
		}

		// Pop cell from return stack.
		void rpop() {
			rStack.pop();
		}

		/****

		----

		Exceptions
		----------

		Forth provides the `ABORT` and `ABORT"` words, which interrupt execution and
		return control to the main `QUIT` loop.  I will implement this functionality
		using a C++ std::exception to return control to the top-level interpreter.

		The C++ functions `abort()` and `abortMessage()` defined here are the first
		primitive functions that will be exposed as Forth words.  For each such word, I
		will spell out the Forth name of the primitive in all-caps, and provide a Forth
		comment showing the stack effects.  For words described in the standards, I
		will generally not provide any more information, but for words that are not
		standard words, I'll provide a brief description.

		****/
		public:
		class AbortException : public std::runtime_error {
		public:
			AbortException(const std::string& msg) : std::runtime_error(msg) {}
			AbortException(const char* msg) : std::runtime_error(msg) {}
		};
		private:
			enum errorCodes {
				errorAbort = -1,
				errorAbortQuote = -2,
				errorStackOverflow =-3,
				errorStackUnderflow=-4,
				errorRStackOverflow=-5,
				errorRStackUnderflow=-6,
				errorInvalidAddress=-9,
				errorDivisionByZero=-10,
				errorUndefinedWord=-13,
				errorParsedStringOverflow=-18,
				errorUnsupportedOperation=-21,
				errorAdressAlignment=-23,
				errorControlStackOverflow=-52,
				errorAllocate=-59,
				errorFree=-60,
				errorResize=-61,
				errorCloseFile=-62,
				errorCreateFile=-63,
				errorDeleteFile=-64,
				errorFilePosition=-65,
				errorFlushFile=-68,
				errorOpenFile=-69,
				errorReadFile=-70,
				errorReadLine=-71,
				errorRenameFile=-72,
				errorWriteFile=-75,
				errorWriteLine=-76
			} ;
			/*
Code Reserved for	Code Reserved for
-1	ABORT
-2	ABORT"
-3	stack overflow
-4	stack underflow
-5	return stack overflow
-6	return stack underflow
-7	do-loops nested too deeply during execution
-8	dictionary overflow
-9	invalid memory address
-10	division by zero
-11	result out of range
-12	argument type mismatch
-13	undefined word
-14	interpreting a compile-only word
-15	invalid FORGET
-16	attempt to use zero-length string as a name
-17	pictured numeric output string overflow
-18	parsed string overflow
-19	definition name too long
-20	write to a read-only location
-21	unsupported operation
(e.g., AT-XY on a too-dumb terminal)
-22	control structure mismatch
-23	address alignment exception
-24	invalid numeric argument
-25	return stack imbalance
-26	loop parameters unavailable
-27	invalid recursion
-28	user interrupt
-29	compiler nesting
-30	obsolescent feature
-31	>BODY used on non-CREATEd definition
-32	invalid name argument (e.g., TO name)
-33	block read exception
-34	block write exception
-35	invalid block number
-36	invalid file position
-37	file I/O exception
-38	non-existent file
-39	unexpected end of file
-40	invalid BASE for floating point conversion
-41	loss of precision
-42	floating-point divide by zero
-43	floating-point result out of range
-44	floating-point stack overflow
-45	floating-point stack underflow
-46	floating-point invalid argument
-47	compilation word list deleted
-48	invalid POSTPONE
-49	search-order overflow
-50	search-order underflow
-51	compilation word list changed
-52	control-flow stack overflow
-53	exception stack overflow
-54	floating-point underflow
-55	floating-point unidentified fault
-56	QUIT
-57	exception in sending or receiving a character
-58	[IF], [ELSE], or [THEN] exception
-59	ALLOCATE
-60	FREE
-61	RESIZE
-62	CLOSE-FILE
-63	CREATE-FILE
-64	DELETE-FILE
-65	FILE-POSITION
-66	FILE-SIZE
-67	FILE-STATUS
-68	FLUSH-FILE
-69	OPEN-FILE
-70	READ-FILE
-71	READ-LINE
-72	RENAME-FILE
-73	REPOSITION-FILE
-74	RESIZE-FILE
-75	WRITE-FILE
-76	WRITE-LINE
-77	Malformed xchar
-78	SUBSTITUTE
-79	REPLACES
*/
		void initClass(){
			// clear datastack
			dStack.resetStack();
			// clear return stack
			rStack.resetStack();
			returnStack.resetStack();
			// clear floating point stack
			// clear control stacks
			controlStackLoops.resetStack();
			// control stack for IF ELSE THEN Begin While Repeat Until Again
			controlStackIf_Begin.resetStack();
			// clear excepion stack
			catchStack.resetStack();
			// clear source/evaluation stack
			while (RestoreInput()); // returns depth of the stack decremented by 1 every call . Or returns 0; 
			// next operation to 0
			next_command = 0;
			// interpretation state
			InterpretState = InterpretSource;
			// SOURCE-ID =0
		}
		void throwMessage(const std::string &msg, enum errorCodes ec){
			initClass();
			std::string buffer{};
			moveFromDataSpace(buffer, getSourceAddress(), getSourceBufferSize());
			throw AbortException(msg + "\n" + buffer + "\n Offset " + std::to_string(getSourceBufferOffset()));
		}
		// ABORT ( i*x -- ) ( R: j*x -- )
		void abort() {
			throwMessage("Abort",errorAbort);
		}
		// ABORT-MESSAGE ( i*x c-addr u -- ) ( R: j*x -- )
		// 
		// Not a standard word.
		//
		// Same semantics as the standard ABORT", but takes a string address and length
		// instead of parsing the message string.
		void abortMessage() {
			auto count = SIZE_T(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop()); pop();
			std::string message{};
			assignStringFromCaddress(message,caddr, count);
			// same action as abort()
			throwMessage(message,errorAbortQuote);
		}

		/****

		----

		Runtime Safety Checks
		---------------------

		Old-school Forths are implemented by super-programmers who never make coding
		mistakes and so don't want the overhead of bounds-checking or other nanny
		hand-holding.  However, I'm just a dumb C++ programmer, and I'd like some help
		to catch mistakes.

		To that end, I have a set of macros and functions that verify that we have the
		expected number of arguments available on our stacks, that we aren't going to
		run off the end of an array, that we aren't going to try to divide by zero, and
		so on.

		You can define the macro `FORTHSCRIPTCPP_SKIP_RUNTIME_CHECKS` to generate an
		executable that doesn't include these checks, so when you have a fully debugged
		Forth application you can run it on that optimized executable for improved
		performance.

		When the `FORTHSCRIPTCPP_SKIP_RUNTIME_CHECKS` macro is not defined, these macros will
		check conditions and throw an `AbortException` if the assertions fail.  I won't
		go into the details of these macros here.  Later we will see them used in the
		definitions of our primitive Forth words.

		****/
#define RUNTIME_NO_OP()                      do { } while (0)

#ifdef FORTHSCRIPTCPP_SKIP_RUNTIME_CHECKS

#define RUNTIME_ERROR(msg)                   RUNTIME_NO_OP()
#define RUNTIME_ERROR_IF(cond, msg)          RUNTIME_NO_OP()
#define REQUIRE_DSTACK_DEPTH(n, name)        RUNTIME_NO_OP()
#define REQUIRE_DSTACK_AVAILABLE(n, name)    RUNTIME_NO_OP()
#define REQUIRE_RSTACK_DEPTH(n, name)        RUNTIME_NO_OP()
#define REQUIRE_RSTACK_AVAILABLE(n, name)    RUNTIME_NO_OP()
#define REQUIRE_ALIGNED(addr, name)          RUNTIME_NO_OP()
#define REQUIRE_VALID_HERE(name)             RUNTIME_NO_OP()
#define REQUIRE_DATASPACE_AVAILABLE(n, name) RUNTIME_NO_OP()

#else

#define RUNTIME_ERROR(msg,ec)                   do { throwMessage(msg,ec); } while (0)
#define RUNTIME_ERROR_IF(cond, msg,ec)          do { if (cond) RUNTIME_ERROR(msg,ec); } while (0)
#define REQUIRE_DSTACK_DEPTH(n, name)        requireDStackDepth(n, name)
#define REQUIRE_DSTACK_AVAILABLE(n, name)    requireDStackAvailable(n, name)
#define REQUIRE_RSTACK_DEPTH(n, name)        requireRStackDepth(n, name)
#define REQUIRE_RSTACK_AVAILABLE(n, name)    requireRStackAvailable(n, name)
#define REQUIRE_CSTACK_DEPTH(n, name)        requireCStackDepth(n, name)
#define REQUIRE_CSTACK_AVAILABLE(n, name)    requireCStackAvailable(n, name)
#define REQUIRE_ALIGNED(addr, name)          RUNTIME_NO_OP()
//#define REQUIRE_ALIGNED(addr, name)          checkAligned(addr, name)
#define REQUIRE_VALID_HERE(name)             checkValidHere(name)
#define REQUIRE_DATASPACE_AVAILABLE(n, name) requireDataSpaceAvailable(n, name)

		template<typename T>
		void checkAligned(T addr, const char* name) {
			RUNTIME_ERROR_IF((CELL(addr) % CellSize) != 0,
				std::string(name) + ": unaligned address",errorAdressAlignment);
		}

		void requireDStackDepth(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF(dStack.stackDepth() < static_cast<std::ptrdiff_t>(n),
				std::string(name) + ": stack underflow",errorStackUnderflow);
		}

		void requireDStackAvailable(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF(!dStack.availableStack(n),
				std::string(name) + ": stack overflow",errorStackOverflow);
		}

		void requireCStackDepth(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF(controlStackIf_Begin.stackDepth() < static_cast<std::ptrdiff_t>(n),
				std::string(name) + ": control stack underflow",errorControlStackOverflow);
		}

		void requireCStackAvailable(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF(!controlStackIf_Begin.availableStack(n),
				std::string(name) + ": control stack overflow",errorControlStackOverflow);
		}

		void requireRStackDepth(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF(rStack.stackDepth() < std::ptrdiff_t(n),
				std::string(name) + ": return stack underflow",errorRStackUnderflow);
		}

		void requireRStackAvailable(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF(!rStack.availableStack(n),
				std::string(name) + ": return stack overflow",errorRStackOverflow);
		}

		void checkValidHere(const char* name) {
			RUNTIME_ERROR_IF(getDataPointer() < VirtualMemory[vmSegmentDataSpace].start || 
				VirtualMemory[vmSegmentDataSpace].end <= getDataPointer(),
				std::string(name) + ": HERE outside data space "+std::to_string(getDataPointer()),errorInvalidAddress);
		}

		void requireDataSpaceAvailable(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF((getDataPointer() + n) >= VirtualMemory[vmSegmentDataSpace].end , //dataSpace.size(),
				std::string(name) + ": data space overflow", errorInvalidAddress);
		}

#endif // FORTHSCRIPTCPP_SKIP_RUNTIME_CHECKS

		/****

		----

		Forth Primitives
		----------------

		Now I will start defining the primitive operations that are exposed as Forth
		words.  You can think of these as the opcodes of a virtual Forth processor.
		Once we have our primitive operations defined, we can then write definitions in
		Forth that use these primitives to build more-complex words.

		Each of these primitives is a function that takes no arguments and returns no
		result, other than its effects on the Forth data stack, return stack, and data
		space.  Such a function can be assigned to the `code` field of a `Definition`.

		When changing the stack, the primitives don't change the stack depth any more
		than necessary.  For example, `PICK` just replaces the top-of-stack value with
		a different value, and `ROLL` uses `std::memmove()` to rearrange elements
		rather than individually popping and pushng them.

		You can peek ahead to the `definePrimitives()` function to see how these
		primitives are added to the Forth dictionary.

		Forth Stack Operations
		----------------------

		Let's start with some basic Forth stack manipulation words.  These differ from
		the push/pop/rpush/rpop/etc. primitives above in that they are intended to be
		called from Forth code rather than from the C++ kernel code.  So I include
		runtime checks and use the stacks rather than passing arguments or returning
		values via C++ call/return mechanisms.

		Note that for C++ functions that implement primitive Forth words, I will
		include the Forth names and stack effects in comments. You can look up the
		Forth names in the standards to learn what these words are supposed to do.

		****/

		// DEPTH ( -- +n )
		void depth() {
			REQUIRE_DSTACK_AVAILABLE(1, "DEPTH");
			push(static_cast<Cell>(dStack.stackDepth()));
		}

		// DROP ( x -- )
		void drop() {
			REQUIRE_DSTACK_DEPTH(1, "DROP");
			pop();
		}

		// 2DROP ( x x -- )
		void drop2() {
			REQUIRE_DSTACK_DEPTH(2, "2DROP");
			pop();
			pop();
		}

		// 2DUP ( x y -- x y x y )
		void dup2() {
			REQUIRE_DSTACK_DEPTH(2, "2DUP");
			REQUIRE_DSTACK_AVAILABLE(2, "2DUP");
			dStack.push(dStack.getTop(1));
			dStack.push(dStack.getTop(1));
		}
		// DUP ( x  -- x x )
		void dup() {
			REQUIRE_DSTACK_DEPTH(1, "DUP");
			REQUIRE_DSTACK_AVAILABLE(1, "DUP");
			dStack.push(dStack.getTop(0));
		}
		// 1+ ( x x0 u -- x+1 )
		void plus1() {
			REQUIRE_DSTACK_DEPTH(1, "1+");
			dStack.setTop(dStack.getTop(0)+1);
		}

		// PICK ( xu ... x1 x0 u -- xu ... x1 x0 xu )
		void pick() {
			REQUIRE_DSTACK_DEPTH(1, "PICK");
			auto index = dStack.getTop();
			REQUIRE_DSTACK_DEPTH(index + 2, "PICK");
			dStack.setTop(dStack.getTop(index + 1));
		}

		// ROLL ( xu xu-1 ... x0 u -- xu-1 ... x0 xu )
		void roll() {
			REQUIRE_DSTACK_DEPTH(1, "ROLL");
			auto n = dStack.getTop(); pop();
			if (n > 0) {
				REQUIRE_DSTACK_DEPTH(n + 1, "ROLL");
				auto x = dStack.getTop(n);
				dStack.moveStack(n, n - 1, n);
				dStack.setTop(x);
			}
		}
		// rot ( X Y Z -- Y Z X  )
		void rot() {
			REQUIRE_DSTACK_DEPTH(3, "ROT");
			auto n = 2;
			if (n > 0) {
				auto x = dStack.getTop(n);
				dStack.moveStack(n, n - 1, n);
				dStack.setTop(x);
			}
		}
		// swap ( X Y  -- Y  X  )
		void swap() {
			REQUIRE_DSTACK_DEPTH(2, "SWAP");
			auto n = 1;
			if (n > 0) {
				auto x = dStack.getTop(n);
				dStack.moveStack(n, n - 1, n);
				dStack.setTop(x);
			}
		}

		// >R ( x -- ) ( R:  -- x )
		void toR() {
			REQUIRE_DSTACK_DEPTH(1, ">R");
			REQUIRE_RSTACK_AVAILABLE(1, ">R");
			rpush(dStack.getTop()); pop();
		}

		// R> ( -- x ) ( R: x -- )
		void rFrom() {
			REQUIRE_RSTACK_DEPTH(1, "R>");
			REQUIRE_DSTACK_AVAILABLE(1, "R>");
			push(rStack.getTop()); rpop();
		}

		// R@ ( -- x ) ( R: x -- x )
		void rFetch() {
			REQUIRE_RSTACK_DEPTH(1, "R@");
			REQUIRE_DSTACK_AVAILABLE(1, "R@");
			push(rStack.getTop());
		}

		/****

		Another important set of Forth primitives are those for reading and writing
		values in data space.

		****/

		// ! ( x a-addr -- )
		void store() {
			REQUIRE_DSTACK_DEPTH(2, "!");
			auto aaddr = AADDR(dStack.getTop()); pop();
			REQUIRE_ALIGNED(aaddr, "!");
			auto x = dStack.getTop(); pop();
			setDataCell( aaddr , x );
		}

		// @ ( a-addr -- x )
		void fetch() {
			REQUIRE_DSTACK_DEPTH(1, "@");
			auto aaddr = AADDR(dStack.getTop());
			if (aaddr > 0x10000 && aaddr < 0xefffffff){
				aaddr = aaddr;
			}
			REQUIRE_ALIGNED(aaddr, "@");
			dStack.setTop(getDataCell(aaddr));
		}

		// c! ( char c-addr -- )
		void cstore() {
			REQUIRE_DSTACK_DEPTH(2, "C!");
			auto caddr = CADDR(dStack.getTop()); pop();
			auto x = static_cast<Char>(dStack.getTop()); pop();
			setDataChar(caddr, x);
			
		}

		// c@ ( c-addr -- char )
		void cfetch() {
			REQUIRE_DSTACK_DEPTH(1, "C@");
			auto caddr = CADDR(dStack.getTop());
			dStack.setTop(static_cast<Cell>(getDataChar( caddr )));
		}

		// COUNT ( c-addr1 -- c-addr2 u )
		void count() {
			REQUIRE_DSTACK_DEPTH(1, "COUNT");
			REQUIRE_DSTACK_AVAILABLE(1, "COUNT");
			auto caddr = CADDR(dStack.getTop());
			auto count = static_cast<Cell>(getDataChar(caddr));
			dStack.setTop(CELL(caddr + 1));
			push(count);
		}
		// -TRAILING ( c-addr1 u1 -- c-addr2 u2 )
		void dashtrailing() {
			REQUIRE_DSTACK_DEPTH(2, "COUNT");
			auto caddr = CADDR(dStack.getTop(1));
			SCell count = dStack.getTop(0);
			while(count>0){
			auto trailingChar = (getDataChar(caddr+count-1));
			if(trailingChar==' '){
				count--;
			} else {
				break;
			}
			}
			//Nothing done
			dStack.setTop(static_cast<Cell>(count));
		}

		/****

		Next, I'll define some primitives for accessing and manipulating the data-space
		pointer, `HERE`.

		****/

		template<typename T>
		AAddr alignAddress(T addr) {
			auto offset = CELL(addr) % CellSize;
			return (offset == 0) ? AADDR(addr) : AADDR(CADDR(addr) + (CellSize - offset));
		}

		void alignDataPointer() {
			setDataPointer( CADDR(alignAddress(getDataPointer())) );
		}

		// ALIGN ( -- )
		void align() {
			alignDataPointer();
			REQUIRE_VALID_HERE("ALIGN");
		}

		// ALIGNED ( addr -- a-addr )
		void aligned() {
			REQUIRE_DSTACK_DEPTH(1, "ALIGNED");
			dStack.setTop(CELL(alignAddress(dStack.getTop())));
		}

		// HERE ( -- addr )
		void here() {
			REQUIRE_DSTACK_AVAILABLE(1, "HERE");
			push(CELL(getDataPointer()));
		}

		// ALLOT ( n -- )
		void allot() {
			REQUIRE_DSTACK_DEPTH(1, "ALLOT");
			REQUIRE_VALID_HERE("ALLOT");
			REQUIRE_DATASPACE_AVAILABLE(CellSize, "ALLOT");
			incDataPointer( static_cast<SCell>( dStack.getTop() ) ); pop();
		}

		// CELLS ( n1 -- n2 )
		void cells() {
			REQUIRE_DSTACK_DEPTH(1, "CELLS");
			dStack.setTop(static_cast<SCell>(dStack.getTop())*CellSize);
		}

		// Store a cell to data space.
		void data(Cell x) {
			REQUIRE_VALID_HERE(",");
			REQUIRE_DATASPACE_AVAILABLE(CellSize, ",");
			REQUIRE_ALIGNED(getDataPointer(), ",");
			setDataCell((AADDR(getDataPointer())) , x );
			incDataPointer( CellSize );
		}

		// UNUSED ( -- u )
		void unused() {
			REQUIRE_DSTACK_AVAILABLE(1, "UNUSED");
			push(static_cast<Cell>(VirtualMemory[vmSegmentDataSpace].end - getDataPointer()));
		}

		/****

		I could implement memory-block words like `CMOVE`, `CMOVE>` and `FILL` in
		Forth, but speed is often important for these, so I will make them native
		primitives.

		****/

		// CMOVE ( c-addr1 c-addr2 u -- )
		void cMove() {
			REQUIRE_DSTACK_DEPTH(3, "CMOVE");
			auto length = SIZE_T(dStack.getTop()); pop();
			auto dst = CADDR(dStack.getTop()); pop();
			auto src = CADDR(dStack.getTop()); pop();
			for (std::size_t i = 0; i < length; ++i) {
				try {
					dataSpaceSet(dst + i, dataSpaceAt(src + i));
				}
				catch (std::exception &ex) {
					auto err = ex.what();
					auto counter = i;
					auto dst1 = dst;
				}
			}
		}

		// CMOVE> ( c-addr1 c-addr2 u -- )
		void cMoveUp() {
			REQUIRE_DSTACK_DEPTH(3, "CMOVE>");
			auto length = SIZE_T(dStack.getTop()); pop();
			auto dst = CADDR(dStack.getTop()); pop();
			auto src = CADDR(dStack.getTop()); pop();
			for (std::size_t i = 0; i < length; ++i) {
				auto offset = length - i - 1;
				dataSpaceSet(dst + offset, dataSpaceAt(src + offset));
			}
		}

		// FILL ( c-addr u char -- )
		void fill() {
			REQUIRE_DSTACK_DEPTH(3, "FILL");
			auto ch = static_cast<Char>(dStack.getTop()); pop();
			auto length = SIZE_T(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop()); pop();
			for (std::size_t i = 0; i < length; ++i) {
				dataSpaceSet(caddr + i, ch);
			}
		}

		// COMPARE ( c-addr1 u1 c-addr2 u2 -- n )
		void compare(){
			REQUIRE_DSTACK_DEPTH(4, "COMPARE");
			auto length2 = SIZE_T(dStack.getTop()); pop();
			auto caddr2 = CADDR(dStack.getTop()); pop();
			auto length1 = SIZE_T(dStack.getTop()); pop();
			auto caddr1 = CADDR(dStack.getTop()); 
			std::string Word1{};
			moveFromDataSpace(Word1, caddr1, length1);
			std::string Word2{};
			moveFromDataSpace(Word2, caddr2, length2);
			auto result = Word1.compare(Word2);
			dStack.setTop((result < 0) ? -1 : ((result > 0) ? 1 : 0)); 
		}
		/// SEARCH ( c-addr1 u1 c-addr2 u2 -- c-addr3 u3 flag )
		//Search the string specified by c-addr1 u1 for the string specified by c-addr2 u2. 
		//If flag is true, a match was found at c-addr3 with u3 characters remaining. 
		//If flag is false there was no match and c-addr3 is c-addr1 and u3 is u1.
		void search(){
			REQUIRE_DSTACK_DEPTH(4, "SEARCH");
			auto length2 = SIZE_T(dStack.getTop()); pop();
			auto caddr2 = CADDR(dStack.getTop()); 
			auto length1 = SIZE_T(dStack.getTop(1)); 
			auto caddr1 = CADDR(dStack.getTop(2)); 
			std::string Word1{};
			moveFromDataSpace(Word1, caddr1, length1);
			std::string Word2{};
			moveFromDataSpace(Word2, caddr2, length2);
			auto result = Word1.find(Word2,0);
			if(result!=std::string::npos){
				dStack.setTop(True);
				dStack.setTop(1,length1-result);
				dStack.setTop(2,caddr1+result);

			} else {
				dStack.setTop(False);
				// dStack.setTop(1,length1); // not changed
				// dStack.setTop(2,caddr1);	 // not changed
			}
			
		}

		/****

		Next I will define I/O primitives.

		I keep things simple and portable by using C++ iostream objects.

		****/
		public:
		enum InputSource {
			FromString = 0,
			FromStdCin = 1
		};
		private:
		enum InputSource readFromSource{};
		public:
		void setInputSource(enum InputSource source){
			readFromSource = source;
		}

#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
		public:
			enum WriteTarget {
				ToString = 0,
				ToStdCout = 1
			};
		private:
			enum WriteTarget writeToTarget{};
		public:
			void setWriteTarget(enum WriteTarget target){
				writeToTarget = target;
			}

#endif
		private:
		// KEY ( -- char )
			void key() {
				REQUIRE_DSTACK_AVAILABLE(1, "KEY");
				SCell ch = EOF;
				switch (readFromSource) {
				case FromString:
					ch = static_cast<SCell>(std_cin.get());
					break;
				case FromStdCin:
					ch = static_cast<SCell>(std::cin.get());
					break;
				default:
					break;
			}
			push(ch);
		}
		// KEY? ( -- flag )
		void key_question() {
			REQUIRE_DSTACK_AVAILABLE(1, "KEY?");
			Cell flag{} ;
			auto &stream = std::cin;
			switch (readFromSource) {
				case FromString:
					flag = std_cin.rdbuf()->in_avail() > 0 ? True : False;
					break;
				case FromStdCin:
					std::cin.sync();
					std::cin.sync_with_stdio(true);
					std::cin.rdbuf()->pubsync();
					flag = std::cin.rdbuf()->in_avail() > 0 ? True : False;
					break;
				default:
					break;
			}
			push(flag);
		}

		// EMIT ( x -- )
		void emit() {
			REQUIRE_DSTACK_DEPTH(1, "EMIT");
			auto cell = dStack.getTop(); pop();
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			switch (writeToTarget) {
			case ToString:
				std_cout.put(static_cast<char>(cell));
				break;
			case ToStdCout:
				std::cout.put(static_cast<char>(cell)); std::cout.flush();
				break;
			default:
				break;
			}
#endif
		}

		// TYPE ( c-addr u -- )
		void type() {
			REQUIRE_DSTACK_DEPTH(2, "TYPE");
			auto length = static_cast<std::streamsize>(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop()); pop();
			while (length-- > 0){
				char ch = dataSpaceAt(caddr++);
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
				switch (writeToTarget) {
				case ToString:
					std_cout.write(&ch, 1);
					break;
				case ToStdCout:
					std::cout.write(&ch, 1);
				    std::cout.flush();
					break;
				default:
					break;
				}
#endif
			}
		}

		// CR ( -- )
		void cr() {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			switch (writeToTarget) {
			case ToString:
				std_cout << std::endl;
				break;
			case ToStdCout:
				std::cout << std::endl;
				std::cout.flush();
				break;
			default:
				break;
			}
#endif
		}

		// . ( n -- )
		void dot() {
			REQUIRE_DSTACK_DEPTH(1, ".");
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			switch (writeToTarget) {
			case ToString:
				std_cout << SETBASE() << static_cast<SCell>(dStack.getTop());
				break;
			case ToStdCout:
				std::cout << SETBASE() << static_cast<SCell>(dStack.getTop());
				std::cout.flush();
				break;
			default:
				break;
			}
#endif
			pop();
		}

		// U. ( x -- )
		void uDot() {
			REQUIRE_DSTACK_DEPTH(1, "U.");
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			switch (writeToTarget) {
			case ToString:
				std_cout << SETBASE() << dStack.getTop();
				break;
			case ToStdCout:
				std::cout << SETBASE() << dStack.getTop();
				std::cout.flush();
				break;
			default:
				break;
			}
#endif
			pop();
		}

		// .R ( n1 n2 -- )
		void dotR() {
			REQUIRE_DSTACK_DEPTH(2, ".R");
			auto width = static_cast<int>(dStack.getTop()); pop();
			auto n = static_cast<SCell>(dStack.getTop()); pop();
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			switch (writeToTarget) {
			case ToString:
				std_cout << SETBASE() << std::setw(width) << n;
				break;
			case ToStdCout:
				std::cout << SETBASE() << std::setw(width) << n;
				std::cout.flush();
				break;
			default:
				break;
			}
#endif
		}

		// BASE ( -- a-addr )
		void base() {
			REQUIRE_DSTACK_AVAILABLE(1, "BASE");
			push(CELL(VarOffsetNumericBase));
		}

		// SOURCE ( -- c-addr u )
		void source() {
			REQUIRE_DSTACK_AVAILABLE(2, "SOURCE");
			push(getDataCell(VarOffsetSourceAddress)); // bug it is not content of variable, it is address of variable
			//auto &sourceBufferVirtual = VirtualMemory.at(vmSegmentSourceBuffer).segment;
			//push(sourceBufferVirtual.size());
			push(getDataCell(VarOffsetSourceSize));
		}
		// SOURCE-ID ( -- n )
		Cell sourceid{};
		void sourcedashid() {
			REQUIRE_DSTACK_AVAILABLE(1, "SOURCE-ID");
			push(sourceid); // bug it is not content of variable, it is address of variable
		}
		void setSourceId(Cell value) {
			sourceid = value;
		}
		Cell getSourceId() {
			return sourceid;
		}

		// >IN ( -- a-addr )
		void toIn() {
			REQUIRE_DSTACK_AVAILABLE(1, ">IN");
			push(VarOffsetSourceOffset);
		}
		// PAD ( -- c-addr )
		void pad() {
			REQUIRE_DSTACK_AVAILABLE(1, "PAD");
			push(CELL(VarOffsetPadBuffer));
		}

		/****

		`REFILL` reads a line from the user input device.  If successful, it puts the
		result into `sourceBuffer`, sets `sourceOffset` to 0, and pushes a `TRUE` flag
		onto the stack.  If not successful, it pushes a `FALSE` flag.

		This uses GNU Readline if configured to do so.  Otherwise it uses the C++
		`std::getline()` function.

		****/

		// REFILL ( -- flag )
		void refill() { 
			REQUIRE_DSTACK_AVAILABLE(1, "REFILL");
			inputBufferStringsCurrent++;
			if (inputBufferStringsCurrent < inputBufferStrings.size()){
				if (setSourceBuffer()){
					push(True);
				}
				else {
					push(False);
				}
			}
			else {
				push(False);
			}
		}

		/****

		`ACCEPT` is similar to `REFILL`, but puts the result into a caller-supplied
		buffer.

		****/

		// ACCEPT ( c-addr +n1 -- +n2 )
		void accept() {
			REQUIRE_DSTACK_AVAILABLE(2, "ACCEPT");
			auto bufferSize = SIZE_T(dStack.getTop()); pop();
			auto buffer = CAddr(dStack.getTop());
			std::string line{};
			switch (readFromSource) {
			case FromString:
				std::getline(std_cin, line);				break;
			case FromStdCin:
				std::getline(std::cin, line);				break;
			default:
				break;
			}
				auto copySize = std::min(line.length(), bufferSize);
				for (std::size_t i = 0; i < copySize; ++i) {
					dataSpaceSet(buffer + i, line.at(i));
				}
				dStack.setTop(static_cast<Cell>(copySize));
		}

		/****

		The text interpreter and other Forth words use `WORD` to parse a
		blank-delimited sequence of characters from the input.  `WORD` skips any
		delimiters at the current input position, then reads characters until it finds
		the delimiter again.  It returns the address of a buffer with the length in the
		first byte, followed by the characters that made up the word.

		In a few places later in the C++ code, you will see the call sequence `bl();
		word(); count();`.  This corresponds to the Forth phrase `BL WORD COUNT`, which
		is how Forth code typically reads a space-delimited word from the input and
		get its address and length.

		The standards specify that the `WORD` buffer must contain a space character
		after the character data, but I'm not going to worry about this obsolescent
		requirement.

		****/

		// WORD ( char "<chars>ccc<char>" -- c-addr )
		void  word() {
			REQUIRE_DSTACK_DEPTH(1, "WORD");
			auto delim = static_cast<char>(dStack.getTop());
			/****

			I need a buffer to store the result of the Forth `WORD` word.  As with the
			input buffer, I use a `std::string` so I don't need to worry about memory
			management.

			Note that while this is a `std::string`, its format is not a typical strings.
			The buffer returned by `WORD` has the word length as its first character.
			That is, it is a Forth _counted string_.

			****/

			std::string wordBuffer{};
			wordBuffer.push_back(0);  // First char of buffer is length.
			try {
				// Skip leading delimiters
				if (getSourceBufferRemain() > 0) {
					auto currentChar = getDataChar(getSourceAddress() + getSourceBufferOffset());
					while (getSourceBufferRemain() > 0 && ((currentChar == delim)
						|| (delim == ' ' && isspace(static_cast<unsigned char>(currentChar)))
						)) {
						incSourceBufferOffset();
						currentChar = getDataChar(getSourceAddress() + getSourceBufferOffset());
					}
				}
				if (getSourceBufferRemain() > 0) {
					auto currentChar = getDataChar(getSourceAddress() + getSourceBufferOffset());
					// Copy characters until we see the delimiter again.
					while (getSourceBufferRemain() >0  
						&& currentChar != delim
						&& !(delim == ' ' && (isspace(static_cast<unsigned char>(currentChar))))
						) {
						wordBuffer.push_back(currentChar);
						incSourceBufferOffset();
						currentChar = getDataChar(getSourceAddress() + getSourceBufferOffset());
					}

					// source point to delimiter, skip it.
					if (getSourceBufferRemain() >0 ) {
						incSourceBufferOffset(); 
					}
				}
#if 0
				else {
					bool exits{ false };
					while (!exits){
						while (getSourceBufferOffset() == inputSize){
							// load next string from input buffer  @bug call mothod to do this work
							++inputBufferStringsCurrent;
							if (inputBufferStringsCurrent < inputBufferStrings.size()){
								setSourceBuffer(inputBufferStrings.at(inputBufferStringsCurrent));
								setSourceBufferOffset(0);
								inputSize = VirtualMemory.at(vmSegmentSourceBuffer).segment.size();
								if (delim == ' ') {
									exits = true; break;
								}
							}
							else {
								if (delim == ' ') {
									exits = true; break;
								}
								throwMessage(std::string("WORD: Did not find expected delimiter \'" +
									std::string(1, delim) + "\'")
									, errorParsedStringOverflow);
							}
						}
						// next char
						if (!exits){
							auto ch = VirtualMemory.at(vmSegmentSourceBuffer).segment.at(getSourceBufferOffset());
							incSourceBufferOffset();
							if ((delim == ' ' && isspace(static_cast<unsigned char>(ch))) || (delim == ch)){
								exits = true;
							}
							else {
								wordBuffer.push_back(ch);
							}
						}
					}
				}
#endif
				if (wordBuffer.size() > 255){
					throwMessage(std::string("WORD: very long word ")
						, errorParsedStringOverflow);

				}
				// Update the count at the beginning of the buffer.
				wordBuffer[0] = static_cast<char>(wordBuffer.size() - 1);
				// copy to fixed buffer
				moveIntoDataSpace(VarOffsetWordBuffer, wordBuffer.c_str(), wordBuffer.size());
				dStack.setTop(CELL(VarOffsetWordBuffer));
			}
			catch (...){
				int a = 1;
			}
		}

		/****

		`PARSE` is similar to `WORD`, but does not skip leading delimiters and provides
		an address-length result.

		****/

		// PARSE ( char "ccc<char>" -- c-addr u )
		void parse() {
			REQUIRE_DSTACK_DEPTH(1, "PARSE");
			REQUIRE_DSTACK_AVAILABLE(1, "PARSE");

			auto delim = static_cast<char>(dStack.getTop());
			/****

			I need a buffer to store the result of the Forth `PARSE` word.  Unlike `WORD`,
			this is a "normal" string and I won't need to store the count at the beginning
			of this buffer.

			****/

			std::string parseBuffer{};
			parseBuffer.clear();
			// Copy characters until we see the delimiter.

			bool exits{ false };
			while (!exits){
				while (getSourceBufferRemain()==0){ 
					// load next string from input buffer
					refill();
					auto flag = dStack.getTop(); pop();
					if(!flag){
							//if (delim == ' ') break;
							throwMessage(std::string("PARSE: Did not find expected delimiter \'" + 
								std::string(1, delim) + "\'")
								, errorParsedStringOverflow);
						}
				}
				if (getSourceBufferRemain() > 0) {
					// next char
					auto ch = getDataChar(getSourceAddress() + getSourceBufferOffset());
					incSourceBufferOffset();
					if ((delim == ' ' && isspace(static_cast<unsigned char>(ch))) || (delim == ch)) {
						exits = true;
					}
					else {
						parseBuffer.push_back(ch);
					}
				}
			}
			auto dataInDataSpace = PutStringToEndOfDataSpace(parseBuffer);

			dStack.setTop(CELL(dataInDataSpace));
			push(static_cast<Cell>(parseBuffer.size()));
		}

		/****

		`BL` puts a space character on the stack.  It is often seen in the phrase `BL
		WORD` to parse a space-delimited word, and will be seen later in the Forth
		definition of `SPACE`.

		****/
		// BL ( -- char )
		void bl() {
			REQUIRE_DSTACK_AVAILABLE(1, "BL");
			push(' ');
		}

		/****

		Next I define arithmetic primitives.

		Note that I need to use the `SCell` type for signed operations, and the `Cell`
		type for unsigned operations.

		****/

		// + ( n1 n2 -- n3 )
		void plus() {
			REQUIRE_DSTACK_DEPTH(2, "+");
			auto n2 = static_cast<SCell>(dStack.getTop()); pop();
			auto n1 = static_cast<SCell>(dStack.getTop());
			dStack.setTop(static_cast<Cell>(n1 + n2));
		}

		// - ( n1 n2 -- n3 )
		void minus() {
			REQUIRE_DSTACK_DEPTH(2, "-");
			auto n2 = static_cast<SCell>(dStack.getTop()); pop();
			auto n1 = static_cast<SCell>(dStack.getTop());
			dStack.setTop(static_cast<Cell>(n1 - n2));
		}

		// * ( n1 n2 -- n3 )
		void star() {
			REQUIRE_DSTACK_DEPTH(2, "*");
			auto n2 = static_cast<SCell>(dStack.getTop()); pop();
			auto n1 = static_cast<SCell>(dStack.getTop());
			dStack.setTop(static_cast<Cell>(n1 * n2));
		}

		// / ( n1 n2 -- n3 )
		void slash() {
			REQUIRE_DSTACK_DEPTH(2, "/");
			auto n2 = static_cast<SCell>(dStack.getTop()); pop();
			auto n1 = static_cast<SCell>(dStack.getTop());
			RUNTIME_ERROR_IF(n2 == 0, "/: zero divisor",errorDivisionByZero);
			dStack.setTop(static_cast<Cell>(n1 / n2));
		}

		// /MOD ( n1 n2 -- n3 n4 )
		void slashMod() {
			REQUIRE_DSTACK_DEPTH(2, "/MOD");
			auto n2 = static_cast<SCell>(dStack.getTop());
			auto n1 = static_cast<SCell>(dStack.getTop(1));
			RUNTIME_ERROR_IF(n2 == 0, "/MOD: zero divisor", errorDivisionByZero);
			auto result = std::ldiv(n1, n2);
			dStack.setTop(1, static_cast<Cell>(result.rem));
			dStack.setTop(static_cast<Cell>(result.quot));
		}
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT
		typedef double FCell;
		const int FCellSize = sizeof(FCell);
		ForthStack<FCell> fStack{};
#define REQUIRE_FSTACK_DEPTH(n, name)        requireFStackDepth(n, name)
#define REQUIRE_FSTACK_AVAILABLE(n, name)    requireFStackAvailable(n, name)
		void requireFStackDepth(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF(fStack.stackDepth() < static_cast<size_t>(n),
				std::string(name) + ": stack underflow", errorStackUnderflow);
		}

		void requireFStackAvailable(std::size_t n, const char* name) {
			RUNTIME_ERROR_IF(!fStack.availableStack(n),
				std::string(name) + ": stack overflow", errorStackOverflow);
		}
		void setDataCell(AAddr pointer, FCell *value){
			for (size_t i = 0; i < sizeof(FCell); ++i){
				dataSpaceSet(pointer + i,  static_cast<unsigned char*>(static_cast<void*>(value))[i]);
			}
		}
		FCell getDataFCell(CAddr pointer){
			FCell value{ 0 };
			for (size_t i = 0; i < sizeof(FCell); ++i){
				static_cast<unsigned char*>(static_cast<void*>(&value))[i] = dataSpaceAt(pointer + i);
			}
			return value;
		}


		// + ( n1 n2 -- n3 )
		void f_plus() {
			REQUIRE_FSTACK_DEPTH(2, "F+");
			auto n2 = static_cast<FCell>(fStack.getTop()); fStack.pop();
			auto n1 = static_cast<FCell>(fStack.getTop());
			fStack.setTop(static_cast<FCell>(n1 + n2));
		}

		// - ( n1 n2 -- n3 )
		void f_minus() {
			REQUIRE_FSTACK_DEPTH(2, "F-");
			auto n2 = static_cast<FCell>(fStack.getTop()); fStack.pop();
			auto n1 = static_cast<FCell>(fStack.getTop());
			fStack.setTop(static_cast<FCell>(n1 - n2));
		}

		// * ( n1 n2 -- n3 )
		void f_star() {
			REQUIRE_FSTACK_DEPTH(2, "F*");
			auto n2 = static_cast<FCell>(fStack.getTop()); fStack.pop();
			auto n1 = static_cast<FCell>(fStack.getTop());
			fStack.setTop(static_cast<FCell>(n1 * n2));
		}

		// / ( n1 n2 -- n3 )
		void f_slash() {
			REQUIRE_FSTACK_DEPTH(2, "F/");
			auto n2 = static_cast<FCell>(fStack.getTop()); fStack.pop();
			auto n1 = static_cast<FCell>(fStack.getTop());
			RUNTIME_ERROR_IF(n2 == 0., "/: zero divisor", errorDivisionByZero);
			fStack.setTop(static_cast<FCell>(n1 / n2));
		}
		// F0= ( -- flag ) ( F: r -- ) or ( r -- flag )
		void f_equals0() {
			REQUIRE_DSTACK_AVAILABLE(1, "F0=");
			REQUIRE_FSTACK_DEPTH(1, "F0=");
			auto n2 = fStack.getTop(); fStack.pop();
			dStack.push(0.0 == n2 ? True : False);
		}
		// F0< ( -- flag ) ( F: r -- ) or ( r -- flag )
		void f_less0() {
			REQUIRE_DSTACK_AVAILABLE(1, "F0<");
			REQUIRE_FSTACK_DEPTH(1, "F0<");
			auto n2 = fStack.getTop(); fStack.pop();
			dStack.push(0.0 > n2 ? True : False);
		}
		// F< ( -- flag ) ( F: r1 r2 -- ) or ( r1 r2 -- flag )
		void f_less() {
			REQUIRE_DSTACK_AVAILABLE(1, "F<");
			REQUIRE_FSTACK_DEPTH(2, "F<");
			auto r2 = fStack.getTop(); fStack.pop();
			auto r1 = fStack.getTop(); fStack.pop();
			dStack.push(r1 < r2 ? True : False);
		}
		// F! ( f-addr -- ) ( F: r -- ) or ( r f-addr -- )
		void f_store() {
			REQUIRE_DSTACK_DEPTH(1, "F!");
			REQUIRE_FSTACK_DEPTH(1, "F!");
			auto aaddr = AADDR(dStack.getTop()); pop();
			REQUIRE_ALIGNED(aaddr, "F!");
			auto x = fStack.getTop(); fStack.pop();
			setDataCell(aaddr, &x);
		}

		// F@ ( f-addr -- ) ( F: -- r ) or ( f-addr -- r )
		void f_fetch() {
			REQUIRE_DSTACK_DEPTH(1, "F@");
			REQUIRE_FSTACK_AVAILABLE(1, "D@");
			auto aaddr = AADDR(dStack.getTop()); pop();
			REQUIRE_ALIGNED(aaddr, "F@");
			fStack.push(getDataFCell(aaddr));
		}

		// D>F ( d -- ) ( F: -- r ) or ( d -- r )
		void f_dtof(){
			REQUIRE_DSTACK_DEPTH(2, "D>F");
			REQUIRE_FSTACK_AVAILABLE(1, "D>F");
			DCell d1(dStack.getTop(1), dStack.getTop()); dStack.pop(); dStack.pop();
			fStack.push(d1.data_.SDcells);
		}
		// F>D ( -- d ) ( F: r -- ) or ( r -- d )
		void f_ftod(){
			REQUIRE_FSTACK_DEPTH(1, "F>D");
			REQUIRE_DSTACK_AVAILABLE(2, "F>D");
			auto n2 = static_cast<FCell>(fStack.getTop()); fStack.pop();
			DCell d1(n2);
			dStack.push(d1.data_.Cells.lo);
			dStack.push(d1.data_.Cells.hi);
		}

		// FALIGN ( -- )
		void f_align() {
			alignDataPointer();
			REQUIRE_VALID_HERE("FALIGN");
		}

		// FALIGNED ( addr -- a-addr )
		void f_aligned() {
			REQUIRE_DSTACK_DEPTH(1, "FALIGNED");
			dStack.setTop(CELL(alignAddress(dStack.getTop())));
		}

		void f_floats() {
			REQUIRE_DSTACK_DEPTH(1, "FLOATS");
			dStack.setTop(static_cast<SCell>(dStack.getTop())*FCellSize);
		}
		// FLOAT+ ( f-addr1 -- f-addr2 )
		void f_floatplus() {
			REQUIRE_DSTACK_DEPTH(1, "FLOAT+");
			auto n1 = dStack.getTop(); 
			dStack.setTop(n1+sizeof(FCell));
		}

		// DEPTH ( -- +n )
		void f_depth() {
			REQUIRE_DSTACK_AVAILABLE(1, "FDEPTH");
			push(static_cast<Cell>(fStack.stackDepth()));
		}

		// DROP ( x -- )
		void f_drop() {
			REQUIRE_FSTACK_DEPTH(1, "FDROP");
			fStack.pop();
		}

		// DUP ( x  -- x x )
		void f_dup() {
			REQUIRE_FSTACK_DEPTH(1, "FDUP");
			REQUIRE_FSTACK_AVAILABLE(1, "FDUP");
			fStack.push(fStack.getTop(0));
		}

		// FPICK ( xu ... x1 x0 u -- xu ... x1 x0 xu )
		void f_pick() {
			REQUIRE_DSTACK_DEPTH(1, "FPICK");
			auto index = dStack.getTop(); pop();
			REQUIRE_FSTACK_DEPTH(index + 1, "FPICK");
			fStack.push(fStack.getTop(index));
		}

		// FROLL ( xu xu-1 ... x0 u -- xu-1 ... x0 xu )
		void f_roll() {
			REQUIRE_DSTACK_DEPTH(1, "FROLL");
			auto n = dStack.getTop(); pop();
			if (n > 0) {
				REQUIRE_FSTACK_DEPTH(n + 1, "FROLL");
				auto x = fStack.getTop(n);
				fStack.moveStack(n, n - 1, n);
				fStack.setTop(x);
			}
		}
		// frot ( X Y Z -- Y Z X  )
		void f_rot() {
			REQUIRE_FSTACK_DEPTH(3, "FROT");
			auto n = 2;
			if (n > 0) {
				auto x = fStack.getTop(n);
				fStack.moveStack(n, n - 1, n);
				fStack.setTop(x);
			}
		}
		// fswap ( X Y  -- Y  X  )
		void f_swap() {
			REQUIRE_FSTACK_DEPTH(2, "SWAP");
			auto n = 1;
			if (n > 0) {
				auto x = fStack.getTop(n);
				fStack.moveStack(n, n - 1, n);
				fStack.setTop(x);
			}
		}

		void f_fconstant(){
			; // @bug to be done
		}
		void f_fliteral(){
			; // @bug to be done
		}
		// FLOOR ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_floor() {
			REQUIRE_FSTACK_DEPTH(1, "FLOOR");
			auto r1 = fStack.getTop(0);
			r1 = std::floor(r1);
			fStack.setTop(r1);
		}
		// FTRUNK ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_ftrunc() {
			REQUIRE_FSTACK_DEPTH(1, "FTRUNC");
			auto r1 = fStack.getTop(0);
			if (r1 > 0.0){
				r1 = std::floor(r1);
			}
			else {
				r1 = std::ceil(r1);
			}
			fStack.setTop(r1);
		}
		// FROUND ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fround() {
			REQUIRE_FSTACK_DEPTH(1, "FROUND");
			auto r1 = fStack.getTop(0);
			r1 = std::round(r1);
			fStack.setTop(r1);
		}
		// FMAX ( F: r1 r2 -- r3 ) 
		void f_fmax() {
			REQUIRE_FSTACK_DEPTH(2, "FMAX");
			auto r1 = fStack.getTop(0); fStack.pop();
			auto r2 = fStack.getTop(0);
			if (r1 > r2){
				fStack.setTop(r1);
			}
		}
		// FMIN ( F: r1 r2 -- r3 ) 
		void f_fmin() {
			REQUIRE_FSTACK_DEPTH(2, "FMIN");
			auto r1 = fStack.getTop(0); fStack.pop();
			auto r2 = fStack.getTop(0);
			if (r1 < r2){
				fStack.setTop(r1);
			}
		}
		// FNEGATE ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fnegate() {
			REQUIRE_FSTACK_DEPTH(1, "FNEGATE");
			auto r1 = fStack.getTop(0);
			fStack.setTop(-r1);
		}
		// (flit) ( -- x )
		//
		// Not a standard word.
		//
		// This instruction gets the value of the next cell, puts that on the float
		// stack, and then moves the instruction pointer to the next instruction.  It
		// is used by `FLITERAL` and other Forth words that need to specify a cell value
		// to put on the stack during execution.
		void doFLiteral() {
			REQUIRE_FSTACK_AVAILABLE(1, "(flit)");
			fStack.push(getDataFCell(next_command));
			next_command += sizeof(FCell);
		}
		// Store a cell to data space.
		void dataFloat(FCell x) {
			REQUIRE_VALID_HERE("F,");
			REQUIRE_DATASPACE_AVAILABLE(FCellSize, "F,");
			REQUIRE_ALIGNED(getDataPointer(), "F,");
			setDataCell((AADDR(getDataPointer())), &x);
			incDataPointer(FCellSize);
		}

		void toFloat(){
			REQUIRE_DSTACK_AVAILABLE(1, ">FLOAT");
			REQUIRE_DSTACK_DEPTH(2, ">FLOAT");
			REQUIRE_FSTACK_AVAILABLE(1, ">FLOAT");
			auto length = SIZE_T(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop());
			std::string currentWord{};
			moveFromDataSpace(currentWord, caddr, length);
			std::regex regexFloat{ "^[ ]*[-+]?(([0-9]*[\\.]{0,1}[0-9]+)|([0-9]+[\\.]{0,1}[0-9]*))([eEdD][-+]?[0-9]*)?[ ]*$" };
			std::smatch m1{};
			bool found = regex_search(currentWord, m1, regexFloat);
			if (found){
				FCell number = std::stod(currentWord);
				dStack.setTop(True);
				fStack.push(number);
				return;
			}
			std::regex regexSpace{ "^[ ]*$" }; // special case - empty string is zero
			found = regex_search(currentWord, m1, regexSpace);
			if (found){
				FCell number{};
				dStack.setTop(True);
				fStack.push(number);
				return;
			}
			dStack.setTop(False);
		}
		void represent(){
			REQUIRE_DSTACK_AVAILABLE(1, "REPRESENT");
			REQUIRE_DSTACK_DEPTH(2, "REPRESENT");
			REQUIRE_FSTACK_AVAILABLE(1, "REPRESENT");
			toFloat();
			auto flag = (dStack.getTop()); 
			if (flag){
				dStack.setTop(0);
				dStack.push(0);
				dStack.push(False);
			}
			else {
				dStack.setTop(0);
				dStack.push(0);
				dStack.push(False);
			}
		}

		// F~ ( -- flag ) ( F: r1 r2 r3 -- ) 
		void f_ftilda(){
			REQUIRE_DSTACK_AVAILABLE(3, "F~");
			REQUIRE_FSTACK_DEPTH(3, "F~");
			auto r3 = fStack.getTop(0); fStack.pop();
			auto r2 = fStack.getTop(0); fStack.pop();
			auto r1 = fStack.getTop(0); fStack.pop();
			Cell ret = False;
			if (r3 > 0.0){
				if (fabs(r2 - r1) < r3) ret = True;
			}
			else if ((r3 == 0.0) || (r3 == -0.0)){
				if (memcmp(&r1, &r2,sizeof(r1))==0) 
					ret = True;
			} if (r3 < 0.0){
				if (abs(r1 - r2) < (abs(r3)*(abs(r1) + abs(r2)))) ret = True;
			}
			dStack.push(ret);
		}
#endif
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT_EXT
		/// F>S ( -- n ) ( F: r -- ) or ( r -- n )
		/// n is the single-cell signed-integer equivalent of the integer portion of r. 
		/// The fractional portion of r is discarded. An ambiguous condition exists if the integer 
		/// portion of r cannot be represented as a single-cell signed integer.
		void f_fmores() {
			REQUIRE_FSTACK_DEPTH(1, "F>S");
			REQUIRE_DSTACK_AVAILABLE(1, "F>S");
			auto r1 = fStack.getTop(0);fStack.pop();
			dStack.push(SCell(r1));
		}
		/// S>F ( n -- ) ( F: -- r ) or ( n -- r ) r is the floating-point equivalent of the single-cell value n. 
		/// An ambiguous condition exists if n can not be precisely represented as a floating-point value.
		void f_smoref() {
			REQUIRE_DSTACK_DEPTH(1, "S>F");
			REQUIRE_FSTACK_AVAILABLE(1, "S>F");
			auto r1 = dStack.getTop(0);dStack.pop();
			fStack.push(FCell(r1));
		}

		// F ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fabs() {
			REQUIRE_FSTACK_DEPTH(1, "FABS");
			auto r1 = fStack.getTop(0);
			fStack.setTop(fabs(r1));
		}
		void f_fsin() {
			REQUIRE_FSTACK_DEPTH(1, "FSIN");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::sin(r1));
		}
		void f_fcos() {
			REQUIRE_FSTACK_DEPTH(1, "FCOS");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::cos(r1));
		}
		void f_fsincos() {
			REQUIRE_FSTACK_DEPTH(1, "FSINCOS");
			REQUIRE_DSTACK_AVAILABLE(1, "FSINCOS");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::sin(r1));
			fStack.push(std::cos(r1));
		}
		void f_ftan() {
			REQUIRE_FSTACK_DEPTH(1, "FTAN");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::tan(r1));
		}
		void f_fasin() {
			REQUIRE_FSTACK_DEPTH(1, "FASIN");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::asin(r1));
		}
		void f_facos() {
			REQUIRE_FSTACK_DEPTH(1, "FACOS");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::acos(r1));
		}
		void f_fatan() {
			REQUIRE_FSTACK_DEPTH(1, "FATAN");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::atan(r1));
		}
		// ( F: r1 r2 -- r3 ) or ( r1 r2 -- r3 ) r1/r2
		void f_fatan2() {
			REQUIRE_FSTACK_DEPTH(1, "FATAN2");
			auto r2 = fStack.getTop(0); fStack.pop();
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::atan2(r1,r2));
		}
		// FSQRT ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fsqrt() {
			REQUIRE_FSTACK_DEPTH(1, "F");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::sqrt(r1));
		}
		// FEXP ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fexp() {
			REQUIRE_FSTACK_DEPTH(1, "FEXP");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::exp(r1));
		}
		// FEXPM1 ( F: r1 -- r2 ) or ( r1 -- r2 ) exp(x)-1.0
		void f_fexpm1() {
			REQUIRE_FSTACK_DEPTH(1, "FEXPM1");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::expm1(r1));
		}
		// FLN ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fln() {
			REQUIRE_FSTACK_DEPTH(1, "FLN");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::log(r1));
		}
		// FLOG ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_flog() {
			REQUIRE_FSTACK_DEPTH(1, "FLOG");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::log10(r1));
		}
		// FLNP1 ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_flnp1() {
			REQUIRE_FSTACK_DEPTH(1, "FLNP1");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::log1p(r1));
		}
		// F** ( F: r1 r2 -- r3 ) or ( r1 -- r2 )
		void f_fpower() {
			REQUIRE_FSTACK_DEPTH(2, "F**");
			auto r2 = fStack.getTop(0); fStack.pop();
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::pow(r1,r2));
		}		
		// FALOG ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_falog() {
			REQUIRE_FSTACK_DEPTH(1, "FALOG");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::pow(10.0,r1));
		}		
		// FSINH ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fsinh() {
			REQUIRE_FSTACK_DEPTH(1, "FSINH");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::sinh(r1));
		}		
		// FCOSH ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fcosh() {
			REQUIRE_FSTACK_DEPTH(1, "FCOSH");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::cosh(r1));
		}
		// FTANH ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_ftanh() {
			REQUIRE_FSTACK_DEPTH(1, "FTANH");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::tanh(r1));
		}		
		// FASINH ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_fasinh() {
			REQUIRE_FSTACK_DEPTH(1, "FASINH");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::asinh(r1));
		}		
		// FACOSH ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_facosh() {
			REQUIRE_FSTACK_DEPTH(1, "FACOSH");
			auto r1 = fStack.getTop(0);
			fStack.setTop(std::acosh(r1));
		}		
		// F ( F: r1 -- r2 ) or ( r1 -- r2 )
		void f_f() {
			REQUIRE_FSTACK_DEPTH(1, "F");
			auto r1 = fStack.getTop(0);
			fStack.setTop(r1);
		}
		unsigned precision_{};
		// PRECISION (   -- u ) 
		void f_precision() {
			REQUIRE_DSTACK_AVAILABLE(1, "PRECISION");
			auto u1 = precision_;
			dStack.push(u1);
		}
		// SET-PRECISION ( u --  ) 
		void f_setprecision() {
			REQUIRE_DSTACK_DEPTH(1, "SET-PRECISION");
			auto u1 = dStack.getTop(0); dStack.pop();
			precision_ = u1;
		}
		// F. ( n -- )
		void f_fdot() {
			REQUIRE_FSTACK_DEPTH(1, "F.");
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			switch (writeToTarget) {
			case ToString:
				std_cout << SETBASE() << std::fixed << std::setprecision(precision_) << static_cast<FCell>(fStack.getTop());
				break;
			case ToStdCout:
				std::cout << SETBASE() << std::fixed << std::setprecision(precision_) << static_cast<FCell>(fStack.getTop());
				std::cout.flush(); // @bug should we make flush every output
				break;
			default:
				break;
			}
#endif
			fStack.pop();
		}
		// FS. ( n -- )
		void f_fsdot() {
			REQUIRE_FSTACK_DEPTH(1, "FS.");
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			switch (writeToTarget) {
			case ToString:
				std_cout << SETBASE() << std::scientific << std::setprecision(precision_)
					<< static_cast<FCell>(fStack.getTop());
				break;
			case ToStdCout:
				std::cout << SETBASE() << std::scientific << std::setprecision(precision_)
					<< static_cast<FCell>(fStack.getTop());
				std::cout.flush();
				break;
			default:
				break;
			}
#endif
			fStack.pop();
		}
		// FE. ( n -- )
		void f_fedot() {
			REQUIRE_FSTACK_DEPTH(1, "FE.");
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			switch (writeToTarget) {
			case ToString:
				std_cout << SETBASE() << std::scientific << std::setprecision(precision_) << static_cast<FCell>(fStack.getTop());
				break;
			case ToStdCout:
				std::cout << SETBASE() << std::scientific << std::setprecision(precision_) << static_cast<FCell>(fStack.getTop());
				std::cout.flush();
				break;
			default:
				break;
			}
#endif
			fStack.pop();
		}


#endif
		// ENVIRONMENT? ( c-addr u -- false | i * x true )
		void environmentquestion(){
			auto length = SIZE_T(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop());
			std::string currentWord{};
			moveFromDataSpace(currentWord, caddr, length);
			Cell ret = False;
			if (currentWord.compare("/COUNTED-STRING") == 0){
				ret = 255;
			} else 
			if (currentWord.compare("/HOLD") == 0){
					ret = 255;
			}
				else
			if (currentWord.compare("ADDRESS-UNIT-BITS") == 0){
						ret = sizeof(Cell)*8;
					}
					else
						if (currentWord.compare("FLOORED") == 0){
							ret = False;
						}
						else
							if (currentWord.compare("RETURN-STACK-CELLS") == 0){
								ret = 100;
							}
							else
								if (currentWord.compare("STACK-CELLS") == 0){
									ret = 200;
								}
								else
									if (currentWord.compare("CORE") == 0){
										ret = True;
									}
									else
										if (currentWord.compare("EXCEPTION") == 0){
											ret = True;
										}
										else
											if (currentWord.compare("FLOATING") == 0){
												ret = True;
											} 
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT
			else if (currentWord.compare("FLOATING-STACK") == 0){
				ret = 200;
			} else
			if (currentWord.compare("MAX-FLOAT") == 0){
				ret = True;
				fStack.push(std::numeric_limits<FCell>::max());
			}
#endif
			else {
				dStack.setTop(False);
				return;
			}
			dStack.setTop(ret);
			dStack.push(True);

		}
		/****

		Next, I define logical and relational primitives.

		****/

		// AND ( x1 x2 -- x3 )
		void bitwiseAnd() {
			REQUIRE_DSTACK_DEPTH(2, "AND");
			auto x2 = dStack.getTop(); pop();
			dStack.setTop(dStack.getTop() & x2);
		}

		// OR ( x1 x2 -- x3 )
		void bitwiseOr() {
			REQUIRE_DSTACK_DEPTH(2, "OR");
			auto x2 = dStack.getTop(); pop();
			dStack.setTop(dStack.getTop() | x2);
		}

		// XOR ( x1 x2 -- x3 )
		void bitwiseXor() {
			REQUIRE_DSTACK_DEPTH(2, "XOR");
			auto x2 = dStack.getTop(); pop();
			dStack.setTop(dStack.getTop() ^ x2);
		}

		// LSHIFT ( x1 u -- x2 )
		void lshift() {
			REQUIRE_DSTACK_DEPTH(2, "LSHIFT");
			auto n = dStack.getTop(); pop();
			dStack.setTop(dStack.getTop() << n);
		}

		// RSHIFT ( x1 u -- x2 ) most significant bit is zero
		void rshift() {
			REQUIRE_DSTACK_DEPTH(2, "RSHIFT");
			auto n = dStack.getTop(); pop();
			dStack.setTop((dStack.getTop()) >> n);
		}
		// 2/ ( x1 -- x2 ) most significant bit is preserved
		void rshiftBy1() {
			REQUIRE_DSTACK_DEPTH(1, "2/");
			dStack.setTop(static_cast<SCell>(dStack.getTop()) >> 1);
		}

		// = ( x1 x2 -- flag )
		void equals() {
			REQUIRE_DSTACK_DEPTH(2, "=");
			auto n2 = dStack.getTop(); pop();
			dStack.setTop(dStack.getTop() == n2 ? True : False);
		}

		// < ( n1 n2 -- flag )
		void lessThan() {
			REQUIRE_DSTACK_DEPTH(2, "<");
			auto n2 = static_cast<SCell>(dStack.getTop()); pop();
			dStack.setTop(static_cast<SCell>(dStack.getTop()) < n2 ? True : False);
		}

		// > ( n1 n2 -- flag )
		void greaterThan() {
			REQUIRE_DSTACK_DEPTH(2, ">");
			auto n2 = static_cast<SCell>(dStack.getTop()); pop();
			dStack.setTop(static_cast<SCell>(dStack.getTop()) > n2 ? True : False);
		}

		// U< ( u1 u2 -- flag )
		void uLessThan() {
			REQUIRE_DSTACK_DEPTH(2, "U<");
			auto u2 = static_cast<Cell>(dStack.getTop()); pop();
			dStack.setTop(static_cast<Cell>(dStack.getTop()) < u2 ? True : False);
		}

		// U> ( u1 u2 -- flag )
		void uGreaterThan() {
			REQUIRE_DSTACK_DEPTH(2, "U>");
			auto u2 = static_cast<Cell>(dStack.getTop()); pop();
			dStack.setTop(static_cast<Cell>(dStack.getTop()) > u2 ? True : False);
		}

		/****

		Now I will define a few primitives that give access to operating-system and
		environmental data.

		****/

		bool isBye = false;
		// BYE ( -- )
		void bye() {
			isBye = true;
		}
	public:
		bool isFinished() const {
			return isBye;
		}
	private:
		// MS ( u -- )
		void ms() {
			REQUIRE_DSTACK_DEPTH(1, "MS");
			auto period = dStack.getTop(); pop();
			//std::this_thread::sleep_for(std::chrono::milliseconds(period));
		}

		// TIME&DATE ( -- +n1 +n2 +n3 +n4 +n5 +n6 )
		void timeAndDate() {
			REQUIRE_DSTACK_AVAILABLE(6, "TIME&DATE");
			auto t = std::time(0);
			tm *tm;
			tm=std::localtime( &t);
			
			push(static_cast<Cell>(tm?tm->tm_sec:0));
			push(static_cast<Cell>(tm ? tm->tm_min : 0));
			push(static_cast<Cell>(tm ? tm->tm_hour : 0));
			push(static_cast<Cell>(tm ? tm->tm_mday : 1));
			push(static_cast<Cell>(tm ? tm->tm_mon + 1 : 1));
			push(static_cast<Cell>(tm ? tm->tm_year + 1900 : 0));
		}
		// MS@ ( -- n ) time in milliseconds since epoch
		std::chrono::milliseconds executionStarted{};
		void ms_at(){
			std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
				std::chrono::system_clock::now().time_since_epoch()
				);
			push(static_cast<Cell>((ms-executionStarted).count()));

		}

		// UTCTIME&DATE ( -- +n1 +n2 +n3 +n4 +n5 +n6 )
		//
		// Not a standard word.
		//
		// Like TIME&DATE, but returns UTC rather than local time.
		void utcTimeAndDate() {
			REQUIRE_DSTACK_AVAILABLE(6, "UTCTIME&DATE");
			auto t = std::time(0);
			tm *tm{};
			tm=std::gmtime( &t);
			push(static_cast<Cell>(tm? tm->tm_sec:0));
			push(static_cast<Cell>(tm ? tm->tm_min:0));
			push(static_cast<Cell>(tm ? tm->tm_hour : 0));
			push(static_cast<Cell>(tm ? tm->tm_mday : 1));
			push(static_cast<Cell>(tm ? tm->tm_mon + 1 : 1));
			push(static_cast<Cell>(tm ? tm->tm_year + 1900 : 0));
		}

		// .S ( -- )
		void dotS() {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			auto depth = dStack.stackDepth();
			std_cout << SETBASE() << "<" << depth << "> ";
			for (auto i = depth; i > 0; --i) {
				switch (writeToTarget) {
				case ToString:
					std_cout << static_cast<SCell>((dStack.getTop(i - 1))) << " ";
					break;
				case ToStdCout:
					std::cout << static_cast<SCell>((dStack.getTop(i - 1))) << " ";
					std::cout.flush();
					break;
				default:
					break;
				}
			}
#endif
		}

		// .RS ( -- )
		//
		// Not a standard word.
		//
		// Like .S, but prints the contents of the return stack instead of the data
		// stack.
		void dotRS() {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			auto depth = rStack.stackDepth();
			std_cout << SETBASE() << "<<" << depth << ">> ";
			for (auto i = depth; i > 0; --i) {
				switch (writeToTarget) {
				case ToString:
					std_cout << static_cast<SCell>(rStack.getTop(i - 1)) << " ";
					break;
				case ToStdCout:
					std::cout << static_cast<SCell>(rStack.getTop(i - 1)) << " ";
					std::cout.flush();
					break;
				default:
					break;
				}
			}
#endif
		}

		/****

		----

		Inner Interpreter
		-----------------

		A Forth system is said to have two interpreters: an "outer interpreter" which
		reads input and interprets it, and an "inner interpreter" which executes
		compiled Forth definitions.  I will start with the inner interpreter.

		There are basically two kinds of words in a Forth system:

		- primitive code: native subroutines that are executed directly by the CPU
		- colon definition: a sequence of Forth words compiled by `:` (colon), `:NONAME`, or `DOES>`.

		Every defined word has a `code` field that points to native code.  In the case
		of primitive words, the `code` field points to a routine that performs the
		operation.  In the case of a colon definition, the `code` field points to the
		`doColon()` function, which saves the current program state and then starts
		executing the words that make up the colon definition.

		Each colon definition ends with a call to `EXIT`, which sets up a return to the
		colon definition that called the current word.  In many traditional Forths, the
		`EXIT` instruction is implemented as a jump/branch to the next machine-code
		instruction to be executed.  But that's not easy to do in a portable way in
		C++, so my `doColon()` just keeps going until it sees an `EXIT` instruction,
		then returns to the caller without actually executing it.

		****/
		ForthStack<Xt> returnStack{};
		void doColon() {
				returnStack.push(next_command);
				auto defn = executingWord;
				next_command = static_cast<Xt>(definitionsAt(defn).does);
		}

		// EXIT ( -- ) ( R: nest-sys -- )
		void exit() {
				next_command = returnStack.getTop(); returnStack.pop();
		}

		/****

		----

		Compilation
		-----------

		Now that we see how the inner interpreter works, I can define the words that
		compile definitions to be executed by that interpreter.

		The kernel provides three words that can add a word to the dictionary:
		`CREATE`, `:`, and `:NONAME`.  Each of them constructs a `Definition`, fills in
		its `name`, `code`, `parameter`, and `does` field appropriately, and then adds
		it to the `definitions` list.

		`:` and `:NONAME` then put the interpreter into compilation mode.  While in
		compilation mode, the interpreter will add the XT's of non-immediate words to
		the current definition.  This continues until the `;` word ends the definition.

		The word `DOES>` can be used after `CREATE` to define execution semantics for
		that word.  As with `:`, this puts the interpreter into compilation state until
		`:` is encountered.

		****/

		// Return reference to the latest Definition.
		// Undefined behavior if the definitions list is empty.
		Definition& lastDefinition() {
			return definitions.back();
		}

		// LATEST ( -- xt )
		//
		// Not a standard word.
		//
		// Puts the execution token of the most recently CREATEd word on the stack.
		void latest() {
			push(CELL(lastDefinition().numberInVector));
		}

		// STATE ( -- a-addr )
		void state() {
			REQUIRE_DSTACK_AVAILABLE(1, "STATE");
			push(CELL(getIsCompilingAddress())); 
		}

		void doCreate() {
			auto defn = executingWord;
			REQUIRE_DSTACK_AVAILABLE(1, definitionsAt(defn).name.c_str());
			push(CELL(definitionsAt(defn).parameter)); 
		}
		void assignStringFromCaddress(std::string &target,  CAddr source, size_t length){
			target.clear();
			for (size_t i = 0; i < length; ++i){
				target.push_back(getDataChar(source + i));
			}
		}
		// CREATE ( "<spaces>name" -- )  Execution: ( -- a-addr )
		void create() {
			alignDataPointer();
			bl();
			word(); 
			count();
			auto length = SIZE_T(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop()); pop();

			RUNTIME_ERROR_IF(length < 1, "CREATE: could not parse name",errorParsedStringOverflow);
			std::string name{};
			assignStringFromCaddress(name, CADDR(caddr), length);
			definePrimitive(name.c_str(), &Forth::doCreate, false);
		}
		
		// : ( C: "<spaces>name" -- colon-sys )
		void colon() {
			create();
			setIsCompiling(  True );

			auto& latest = lastDefinition();
			latest.code = &Forth::doColon;
			latest.toggleHidden();
		}

		// :NONAME ( C:  -- colon-sys )  ( S:  -- xt )
		void noname() {
			alignDataPointer();
			definePrimitive("", &Forth::doColon, false);
			setIsCompiling( True );
			latest();
		}

		void doDoes() {
			doCreate();
			doColon();
		}

		void setDoes() {
			auto& latest = lastDefinition();
			latest.code = &Forth::doDoes;
			latest.does = AADDR(next_command) + sizeof(next_command); 
		}

		// DOES>
		void does() {
			data(CELL(setDoesXt));
			data(CELL(exitXt));
		}

		// (;) ( -- )
		//
		// Not a standard word.
		//
		// This word is compiled by ; after the EXIT.  It is never executed, but serves
		// as a marker for use in debugging.
		void endOfDefinition() {
			throwMessage("(;) should never be executed",errorUnsupportedOperation);
		}

		// ; ( C: colon-sys -- )
		void semicolon() {
			data(CELL(exitXt));
			data(CELL(endOfDefinitionXt));
			setIsCompiling( False );
			lastDefinition().toggleHidden(); // @bug lastDefinition can be not last if compilation created additional definitions
		}

		// IMMEDIATE ( -- )
		//
		// Unlike the standard specification, my IMMEDIATE toggles the immediacy bit
		// of the most recent definition, rather than always setting it true.
		void immediate() { lastDefinition().toggleImmediate(); }

		/****

		Next I'll define a few "special words".  They are special in that they are used
		to implement features of the inner interpreter, and are not generally used by
		Forth application code.  As a signifier of their special nature, the words'
		names start and end with with parentheses.

		****/

		// (lit) ( -- x )
		//
		// Not a standard word.
		//
		// This instruction gets the value of the next cell, puts that on the data
		// stack, and then moves the instruction pointer to the next instruction.  It
		// is used by `LITERAL` and other Forth words that need to specify a cell value
		// to put on the stack during execution.
		void doLiteral() {
			REQUIRE_DSTACK_AVAILABLE(1, "(lit)");
			push(CELL(getDataCell(next_command)));
			next_command+=sizeof(Cell);
		}

		// (branch) ( -- )
		// 
		// Not a standard word.
		//
		// Used by branching/looping constructs.  Unconditionally adds an offset to
		// `next_command`.  The offset is in the cell following the instruction.
		//
		// The offset is in character units, but must be a multiple of the cell size.
		void branch() {
			auto offset = static_cast<SCell>(getDataCell(next_command));
			next_command += offset ;
		}

		// (zbranch) ( flag -- )
		// 
		// Not a standard word.
		//
		// Used by branching/looping constructinos.  Adds an offset to `next` if the
		// top-of-stack value is zero.  The offset is in the cell following the
		// instruction.  If top-of-stack is not zero, then continue to the next
		// instruction.
		void zbranch() {
			REQUIRE_DSTACK_DEPTH(1, "(zbranch)");
			auto flag = dStack.getTop(); pop();
			if (flag == False)
				branch();
			else
				next_command += sizeof(next_command); 
		}

		/****

		Dictionary
		----------

		The next section contains words that create elements in the `definitions` list,
		look up elements by name, or traverse the list to perform some operation.

		****/

		// Create a new definition with specified name and code.
		void definePrimitive(const char* name, Code code, bool setImmediate) {
			alignDataPointer();

			Definition defn;
			defn.code = code;
			defn.parameter = defn.does = AADDR(getDataPointer());
			defn.name = name;
			definitions.emplace_back(std::move(defn));
			getDefinition(definitions.size() - 1)->numberInVector = definitions.size() - 1;
			if (setImmediate) immediate();
		}

		// Determine whether two names are equivalent, using case-insensitive matching.
		bool doNamesMatch(CAddr name1, const std::string &name2, Cell nameLength) {
			for (std::size_t i = 0; i < nameLength; ++i) {
				if (std::toupper(static_cast<unsigned char>(getDataChar(name1 + i))) != std::toupper(static_cast<unsigned char>(name2[i]))) {
					return false;
				}
			}
			return true;
		}
		bool doNamesMatch(const std::string name1, const std::string &name2, Cell nameLength) {
			for (std::size_t i = 0; i < nameLength; ++i) {
				if (std::toupper(static_cast<unsigned char>(name1[i])) != std::toupper(static_cast<unsigned char>(name2[i]))) {
					return false;
				}
			}
			return true;
		}

		// Find a definition by name.
		Xt findDefinition(CAddr nameToFind, Cell nameLength) {
			if (nameLength == 0)
				return 0;

			for (auto i = definitions.rbegin(); i != definitions.rend(); ++i) {
				auto& defn = *i;
				if (!defn.isFindable())
					continue;
				auto& name = defn.name;
				if (name.length() == nameLength) {
					if (doNamesMatch(nameToFind, name.data(), nameLength)) {
						return defn.numberInVector; 
					}
				}
			}
			return 0;
		}

		// Find a definition by name.
		Xt findDefinition(const std::string& nameToFind) {
			Cell nameLength = nameToFind.size();
			if (nameLength == 0)
				return 0;

			for (auto i = definitions.rbegin(); i != definitions.rend(); ++i) {
				auto& defn = *i;
				if (!defn.isFindable())
					continue;
				auto& name = defn.name;
				if (name.length() == nameLength) {
					if (doNamesMatch(nameToFind, name.data(), nameLength)) {
						return defn.numberInVector; 
					}
				}
			}
			return 0;
		}

		// FIND ( c-addr -- c-addr 0  |  xt 1  |  xt -1 )
		void find() {
			REQUIRE_DSTACK_DEPTH(1, "FIND");
			REQUIRE_DSTACK_AVAILABLE(1, "FIND");
			auto caddr = CADDR(dStack.getTop());
			auto length = static_cast<Cell>(getDataChar(caddr));
			auto name = caddr + 1;
			auto word = findDefinition(name, length);
			if (word == 0) {
				push(0);
			}
			else {
				dStack.setTop(CELL(word));
				push(definitionsAt(word).isImmediate() ? 1 : Cell(-1));
			}
		}
		// SEARCH-WORDLIST ( c-addr -- c-addr 0  |  xt 1  |  xt -1 )
		void searchdashwordlist() {
			REQUIRE_DSTACK_DEPTH(3, "SEARCH-WORDLIST");
			auto wid = CELL(dStack.getTop()); pop();
			auto length = CELL(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop()); pop();
			//auto length = static_cast<Cell>(getDataChar(caddr));
			auto name = caddr;
			auto word = findDefinition(name, length);
			if (word == 0) {
				push(0);
			}
			else {
				dStack.setTop(CELL(word));
				push(definitionsAt(word).isImmediate() ? 1 : Cell(-1));
			}
		}

		// EXECUTE ( i*x xt -- j*x )
		void execute() {
			assert(!"Should not be executed");
			REQUIRE_DSTACK_DEPTH(1, "EXECUTE");
			auto defn = XT(dStack.getTop()); pop();
#ifdef _DEBUG
			traceDebug( definitionsAt(defn).name );
#endif
			definitionsAt(defn).executeDefinition(*this);
		}

		// >BODY ( xt -- a-addr )
		void toBody() {
			REQUIRE_DSTACK_DEPTH(1, ">BODY");
			auto xt = XT(dStack.getTop());
			dStack.setTop(CELL(definitionsAt(xt).parameter));
		}

		// XT>NAME ( xt -- c-addr u )
		//
		// Not a standard word.
		//
		// Gives the name associated with an xt.
		void xtToName() {
			REQUIRE_DSTACK_DEPTH(1, "XT>NAME");
			REQUIRE_DSTACK_AVAILABLE(1, "XT>NAME");
			auto xt = XT(dStack.getTop());
			auto& name = definitionsAt(xt).name;
			auto dataInDataSpace = PutStringToEndOfDataSpace(definitionsAt(xt).name);
			dStack.setTop(CELL(dataInDataSpace));
			push(static_cast<Cell>(name.length()));
		}

		// WORDS ( -- )
		void words() {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			std::stringstream &buf = std_cout;
			auto param = writeToTarget;
			std::for_each(definitions.rbegin(), definitions.rend(), [&buf,&param](Definition& defn) {
				if (defn.isFindable()) {
					switch (param) {
					case ToString:
						buf << defn.name << " ";
						break;
					case ToStdCout:
						std::cout << defn.name << " ";
						std::cout.flush();
						break;
					default:
						break;
					}
				}
			});
#endif
		}

		/****

		SEE
		---

		Most Forth systems provide a word `SEE` that will print out the definition of a word.

		My implementation of this word just walks through the contents of a definition
		and tries to "decompile" each cell.  If the cell contains the XT of a defined
		word, then print that word's name.  Otherwise, it just prints the cell value.

		This generally gives a readable view of the word's definition, but it is not
		exactly equal to the original source text.  For example, for this definition:

		: add-1-and-2 ( -- )   1 2 + . ;

		`SEE add-1-and-2` gives this output:

		: add-1-and-2 (lit) 1 (lit) 2 + . EXIT ;

		It gets even messier when decompiling words that contain branches and std::string
		literals, but it works well as a debugging tool when trying to determine why a
		word is not compiling as expected.

		****/

		// Given a cell that might be an XT, search for it in the definitions list.
		//
		// Returns a pointer to the definition if found, or nullptr if not.
		Xt findXt(Cell x) {
			for (auto i = definitions.rbegin(); i != definitions.rend(); ++i) {
				auto& defn = *i;
				if ((&defn - &*definitions.begin()) == static_cast<Xt>(x))
					return defn.numberInVector; 
			}
			return 0;
		}

		/// Display the words that make up a colon or DOES> definition.
		void seeDoes(AAddr does) {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			while (XT(getDataCell( does )) != endOfDefinitionXt) {
				auto xt = findXt(getDataCell(does));
				if (xt)
					std_cout << " " << definitionsAt(xt).name;
				else
					std_cout << " " << SETBASE() << static_cast<SCell>(getDataCell(does));
				++does;
			}
			std_cout << " ;";
#endif
		}

		// SEE ( "<spaces>name" -- )
		void see() {
			bl(); word(); find();

			auto found = dStack.getTop(); pop();
			if (!found) throwMessage("SEE: undefined word",errorParsedStringOverflow);

			auto defn = getDefinition(XT(dStack.getTop())); pop();
			if ((defn)->code == &Forth::doColon) {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
				std_cout << ": " << (defn)->name;
#endif
				seeDoes((defn)->does);
			}
			else if ((defn)->code == &Forth::doCreate || defn->code == &Forth::doDoes) {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
				std_cout << "CREATE " << defn->name << " ( " << CELL(defn->parameter) << " )";
				if (defn->code == &Forth::doDoes) {
					std_cout << " DOES>";
					seeDoes(defn->does);
				}
#endif
			}
			else {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
				std_cout << ": " << defn->name << " <primitive " << SETBASE() << "addr of proc is not defined " /*CELL((void*)(defn->code))*/ << "> ;";
#endif
			}
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			if (defn->isImmediate()) std_cout << " immediate";
#endif
		};

		// DO ( to from -- )
// ": DO HERE ['] (BRANCH) , 12 , ['] (BRANCH) , 0 , ['] >R , ['] >R ,  ; IMMEDIATE ",

		void loops_do() {
			controlStackLoops.push( getDataPointer() );
			data(branchXt);
			data(CellSize*3);
			data(branchXt);
			data(CellSize);
			data(toR_Xt);
			data(toR_Xt);
		}
		// ?DO
		void loops_question_do() {
			data(DupDupXt);		// 0
			data(EqualsXt);		// 1
			data(zbranchXt);	// 2	
			data(CellSize * 7);	// 3 Jump 3 to 10 goto label2
			data(branchXt);		// 4
			data(3 * CellSize);	// 5 Jump 5 to 8 goto label1
			controlStackLoops.push(getDataPointer());
			data(branchXt);		// 6
			data(CellSize * 3);	// 7
			data(branchXt);		// 8  label1:
			data(CellSize);		// 9
			data(toR_Xt);		// 10 label2:
			data(toR_Xt);
		}
		// LOOP ( -- )
		// : LOOP ['] R> , ['] R> , ['] 1+ , ['] 2DUP , ['] = , ['] (ZBRANCH) , HERE - , ['] 2DROP , ; IMMEDIATE 
		void loops_loop(){
			data(rFrom_Xt);
			data(rFrom_Xt);
			data(Plus1Xt);
			data(DupDupXt);
			data(EqualsXt);
			data(zbranchXt);
			auto cell = controlStackLoops.getTop(); controlStackLoops.pop();
			data(cell - getDataPointer());
			// calculate return point after LEAVE and save it to second BRANCH in DO
			auto offset = getDataPointer() - (cell + CellSize * 3);
			setDataCell(cell + CellSize * 3, offset);
			// clear stack after cycle
			data(DropDropXt);
		}
		// +LOOP ( -- )
		// : +LOOP ['] R> , ['] R> , ['] ROT , ['] + , ['] 2DUP , ['] = , ['] (ZBRANCH) , HERE - ,  ['] 2DROP , ; IMMEDIATE 
		void loops_plusloop(){
#ifdef PLUSLOOP_SIMPLE_AND_DIRTY 
			data(rFrom_Xt);
			data(rFrom_Xt);
			data(rotXt);
			data(plusXt);
			data(DupDupXt);
			data(EqualsXt);
#else 
			data(plusLoopCheckXt);
#endif
			data(zbranchXt);
			auto cell = controlStackLoops.getTop(); controlStackLoops.pop();
			data(cell - getDataPointer());
			// calculate return point after LEAVE and save it to second BRANCH in DO
			auto offset = getDataPointer() - (cell + CellSize * 3);
			setDataCell(cell + CellSize * 3, offset);
			data(DropDropXt);
		}
		// LEAVE ( -- )
		// : +LOOP ['] R> , ['] R> , ['] ROT , ['] + , ['] 2DUP , ['] = , ['] (ZBRANCH) , HERE - ,  ['] 2DROP , ; IMMEDIATE 
		void loops_leave(){
			data(rFrom_Xt);
			data(rFrom_Xt);
			data(branchXt);
			auto cell = controlStackLoops.getTop(); 
			data(cell - getDataPointer()+8);
			data(DropDropXt);
		}
		// PlusLoop_Check
		// : +Loop_check R> , ['] R> , ['] ROT , ['] + , ['] 2DUP , ['] = : (but compare in correct way)
		void loops_plusloop_check() {
			// conditions : first index is the sign of increment (0 => +, 1 => -)
			// second index is the sign of index  (0 => +, 1 => -)
			// third index is the sign of newIndex  (0 => +, 1 => -)
			// value: 1 - compare as usual, 0 - compare considering integer overflow
			static int conditions[2][2][2] = { 
				{ { 1, 0 }, { 1, 1 }},
				{ { 1, 1 }, { 0, 1 }}
			};
			REQUIRE_RSTACK_DEPTH(2, "plusloop_check");
			REQUIRE_DSTACK_DEPTH(1, "plusloop_check");
			REQUIRE_DSTACK_AVAILABLE(2, "plusloop_check");
    		SCell limit = rStack.getTop(); // limit of loop
			SCell index = rStack.getTop(1); // current index of loop
			SCell increment = dStack.getTop(); // increment of loop from stack
			SCell limitMinus1 = limit - 1;   // limit and limit-1 - the border to cross as exit test for loop
			SCell newIndex = index + increment; // if newIndex crossed the border, than the loop is finished
			int signIndex = index > 0 ? 0 : 1;
			int signNewIndex = newIndex > 0 ? 0 : 1;
			int signIncrement = increment > 0 ? 0 : 1;
			int compare = conditions[signIncrement][signIndex][signNewIndex];
			Cell result = 0;
			if (compare == 1){
				if (((index <= limitMinus1) && (newIndex >= limit)) || ((newIndex <= limitMinus1) && (index >= limit))){
					result = -1;
				}
			}
			else {
				if (!(((index <= limitMinus1) && (newIndex >= limit)) || ((newIndex <= limitMinus1) && (index >= limit)))){
					result = -1;
				}
			}
			dStack.setTop(limit);
			dStack.push(newIndex);
			dStack.push(result);
			rStack.pop();
			rStack.pop();
		}
		// I ( -- x ) ( R:  -- )
		void loops_i(){
			REQUIRE_RSTACK_DEPTH(2, "I");
			REQUIRE_DSTACK_AVAILABLE(1, "I");
			push(rStack.getTop(1)); 
		}
		// J ( -- x ) ( R:  -- )
		void loops_j(){
			REQUIRE_RSTACK_DEPTH(4, "J");
		REQUIRE_DSTACK_AVAILABLE(1, "J");
		push(rStack.getTop(3));
		}
		// K ( -- x ) ( R:  -- )
		void loops_k(){
			REQUIRE_RSTACK_DEPTH(6, "K");
		REQUIRE_DSTACK_AVAILABLE(1, "K");
		push(rStack.getTop(5));
		}
		// UNLOOP ( --  ) ( R: x y  -- )
		void loops_unloop(){
		//REQUIRE_RSTACK_DEPTH(2, "UNLOOP");
		if(rStack.stackDepth()>0) rStack.pop();
		if (rStack.stackDepth()>0)rStack.pop();
		}
			
		/*******
		 Begin-While-Repeate and If-Else-Then control stack.
		 orig - forward reference
		 dest - backward reference
		 if  (		--	orig)				condition
		 else (orig1 -- orig2)
		 then (orig -- )
		 ahead (	--	orig) 
		 begin (	--	dest)
		 while ( dest -- orig dest)			condition
		 repeat (orig dest -- )
		 until (dest	--	)				condition
		 again (dest	--	)		
		******/
	//		if
		void ifthenelse_if() {
			data(zbranchXt);
			controlStackIf_Begin.push(getDataPointer());
			data(CellSize );
		}
		//		ahead
		void ifthenelse_ahead() {
			data(branchXt);
			controlStackIf_Begin.push(getDataPointer());
			data(CellSize);
		}

	//			then  
		void ifthenelse_then() {
			auto controlStackSize = controlStackIf_Begin.stackDepth();
			if (controlStackSize > 0){
				auto ifHere = controlStackIf_Begin.getTop();
				setDataCell(ifHere, getDataPointer() - ifHere);
				controlStackIf_Begin.pop();
			}
		}
	//			 else 
		void ifthenelse_else() {
			auto controlStackSize = controlStackIf_Begin.stackDepth();
			data(branchXt);
			auto elseHere = getDataPointer();
			data(CellSize);
			if (controlStackSize > 0){
				auto ifHere = controlStackIf_Begin.getTop();
				setDataCell(ifHere, getDataPointer() - ifHere);
				controlStackIf_Begin.setTop(elseHere);
			}
			else {
				controlStackIf_Begin.push(elseHere); // else without forward reference
			}
		}
		void begin_etc_begin() {
			controlStackIf_Begin.push(getDataPointer()); // -- dest 
		}
// again 
		void begin_etc_again() {
			data(branchXt);
			CAddr AddressAtBegin = controlStackIf_Begin.getTop();
			data(AddressAtBegin-getDataPointer());
			controlStackIf_Begin.pop();
		}
//  until   
		void begin_etc_until() {
			data(zbranchXt);
			CAddr AddressAtBegin = controlStackIf_Begin.getTop();
			data(AddressAtBegin - getDataPointer() );
			controlStackIf_Begin.pop();
		}
// while    
		void begin_etc_while() {
			CAddr AddressAtBegin = controlStackIf_Begin.getTop();
			data(zbranchXt);
			controlStackIf_Begin.setTop(getDataPointer()); // orig
			data(CellSize);
			controlStackIf_Begin.push(AddressAtBegin); // dest
		}
//  repeat   
		void begin_etc_repeat() {
			// resolve dest
			data(branchXt);
			CAddr AddressAtBegin = controlStackIf_Begin.getTop();
			data(AddressAtBegin - getDataPointer());
			controlStackIf_Begin.pop();
			// resolve orig
			auto controlStackSize = controlStackIf_Begin.stackDepth();
			if (controlStackSize > 0){
				CAddr ifHere = controlStackIf_Begin.getTop();
				setDataCell(ifHere, getDataPointer() - ifHere);
				controlStackIf_Begin.pop();
			}
		}

		// CS-PICK ( C: destu ... orig0 | dest0 -- destu ... orig0 | dest0 destu ) ( S: u -- )
		void csdashpick() {
			REQUIRE_DSTACK_DEPTH(1, "CS-PICK");
			auto index = dStack.getTop(); pop();
			REQUIRE_CSTACK_DEPTH(index+1, "CS-PICK");
			REQUIRE_CSTACK_AVAILABLE(1, "CS-PICK");
			controlStackIf_Begin.push(controlStackIf_Begin.getTop(index));
			//auto cell = controlStackLoops.getTop(); controlStackLoops.pop();
		}
		// CS-ROLL ( C: origu | destu origu-1 | destu-1 ... orig0 | dest0 -- origu-1 | destu-1 ... orig0 | dest0 origu | destu ) ( S: u -- )
		void csdashroll() {
			REQUIRE_DSTACK_DEPTH(1, "CS-ROLL");
			auto index = dStack.getTop(); pop();
			if(index>0){
			REQUIRE_CSTACK_DEPTH(index+1, "CS-ROLL");
			 auto newTop=controlStackIf_Begin.getTop(index);
			 for(auto i=index;i>0;--i){
			  controlStackIf_Begin.setTop(i,controlStackIf_Begin.getTop(i-1));
			 }
			 controlStackIf_Begin.setTop(newTop);
			}
			//auto cell = controlStackLoops.getTop(); controlStackLoops.pop();
		}
		// N>R ( i * n +n -- ) ( R: -- j * x +n )
		void nmorer() {
			REQUIRE_DSTACK_DEPTH(1, "N>R");
			auto index = dStack.getTop(); pop();
			REQUIRE_DSTACK_DEPTH(index, "N>R");
			REQUIRE_RSTACK_AVAILABLE(index+1, ">R");
			for(size_t i=0;i<index;++i){
			 rpush(dStack.getTop()); pop();
			}
			rpush(index);
		}

		// NR> ( -- x ) ( R: x -- )
		void nrmore() {
			REQUIRE_RSTACK_DEPTH(1, "NR>");
			auto index = rStack.getTop(); rpop();
			REQUIRE_RSTACK_DEPTH(index, "NR>");
			REQUIRE_DSTACK_AVAILABLE(index+1, "R>");
			for(size_t i=0;i<index;++i){
			 push(rStack.getTop()); rpop();
			}
			push(index);
		}
		void synonym() {
			bl(); word();
			count();
			auto length = SIZE_T(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop()); pop();
			if (length > 0) {
				std::string newWord{};
				moveFromDataSpace(newWord, caddr, length);
				bl(); word(); dup();
				count();
				length = SIZE_T(dStack.getTop()); pop();
				caddr = CADDR(dStack.getTop()); pop();
				if (length > 0) {
					std::string currentWord{};
					moveFromDataSpace(currentWord, caddr, length);
					find();
					auto found = static_cast<int>(dStack.getTop()); pop();
					if (found) {
						auto xt = XT(dStack.getTop()); pop();
						auto definition = *getDefinition(xt);
						definition.name = newWord;
						definitions.emplace_back(std::move(definition));
						getDefinition(definitions.size() - 1)->numberInVector = definitions.size() - 1;
					}
					else {
						pop(); // remove from stack. not found , ambigous state
					}
				}

			}
		}
		
		// noop ( -- ) no operation
		void noop() {
			;
		}

		// BracketElse
		/* " : [ELSE] 1 BEGIN " // level 
 "       BEGIN BL WORD COUNT DUP WHILE " //              level adr len 
 "       2DUP S\" [IF]\" COMPARE 0= IF "	//              level adr len 
 "             2DROP 1+ " //                             level' 
 "          ELSE " //                                   level adr len 
 "            2DUP S\" [ELSE]\" COMPARE 0= IF " //	    level adr len 
 "                2DROP 1- DUP IF 1+ THEN "  //          level' 
 " ELSE " //	                                level adr len 
 " S\" [THEN]\" COMPARE 0= IF " //        level 
 "                   1- " //                             level' 
 "               THEN " //
 "             THEN " //
 "           THEN ?DUP 0= IF EXIT THEN " //              level' 
 "       REPEAT 2DROP " //                              	level 
 "   REFILL 0= UNTIL " //                                level 
 "    DROP ; IMMEDIATE ",
 */
		void bracketElse(){
			int level = 1;
			while (true){
				while (getSourceBufferRemain() > 0 ) {
					bl();
					word();
					count();
					auto length = SIZE_T(dStack.getTop()); pop();
					auto caddr = CADDR(dStack.getTop()); pop();
					if (length > 0) {
						std::string currentWord{};
						moveFromDataSpace(currentWord, caddr, length);
						if (currentWord.compare("[IF]")) { level++; }
						else {
							if (currentWord.compare("[ELSE]")) { level--; if (level) level++; }
							else {
								if (currentWord.compare("[THEN]")) { level--; }
								if (level == 0) return;
							}
						}
					}
					else {
						break;
					}
				}
				refill();
			}
		}

		/****

		Outer Interpreter
		-----------------

		The word `INTERPRET` below implements the outer interpreter.  It looks at the
		`sourceBuffer`, and repeats the following until it has processed all the
		characters in the buffer:

		- Parse a space-delimited word.
		- Look up that word in the dictionary.
		- If the word is found:
		- If we are not in compilation mode, or if the word is an immediate word, then execute it.
		- Otherwise (in compilation mode), compile a call to the word.
		- If the word is not found:
		- Try to parse it as a number.
		- If it is a number:
		- If in compilation mode, then compile it as a literal.
		- Otherwise, put the value on the stack.
		- If it is not a number, then signal an error.

		See [section 3.4 of the ANS Forth draft standard][dpans_3_4] for a more complete description
		of the Forth text interpreter.

		[dpans_3_4]: http://forth.sourceforge.net/std/dpans/dpans3.htm#3.4 "3.4 The Forth text interpreter"

		****/

		// Determine whether specified character is a valid numeric digit for current BASE.
		bool isValidDigit(Char c) {
			return isValidDigit(c, getNumericBase());
		}
		bool isValidDigit(Char c, int numericBase) {
			if (numericBase > 10) {
				if (('A' <= c) && (c < ('A' + numericBase - 10)))
					return true;
				if (('a' <= c) && (c < ('a' + numericBase - 10)))
					return true;
			}
			return ('0' <= c) && (c < ('0' + numericBase));
		}

		// Return numeric value associated with a character.
		Cell digitValue(Char c) {
			if (c >= 'a')
				return c - 'a' + 10;
			else if (c >= 'A')
				return c - 'A' + 10;
			else
				return c - '0';
		}

		// >UNUM ( u0 c-addr1 u1 -- u c-addr2 u2 )
		//
		// Not a standard word.
		//
		// This word is similar to the standard >NUMBER, but provides a single-cell
		// result.
		void parseUnsignedNumber(int numericBase) {
			REQUIRE_DSTACK_DEPTH(3, ">UNUM");

			auto length = SIZE_T(dStack.getTop());
			auto caddr = CADDR(dStack.getTop(1));
			auto value = dStack.getTop(2);

			auto i = std::size_t(0);
			while (i < length) {
				auto c = getDataChar(caddr+i);
				if (isValidDigit(c, numericBase)) {
					auto n = digitValue(c);
					value = value * numericBase + n;
					++i;
				}
				else {
					break;
				}
			}

			dStack.setTop(2, value);
			dStack.setTop(1, CELL(caddr + i));
			dStack.setTop(length - i);
		}

		// >NUM ( n c-addr1 u1 -- n c-addr2 u2 )
		//
		// Not a standard word.
		// 
		// Similar to >UNUM, but looks for a '-' character at the beginning, and
		// negates the result if found.
		void parseSignedNumber(int numericBase) {
			REQUIRE_DSTACK_DEPTH(3, ">NUM");

			auto length = SIZE_T(dStack.getTop());
			auto caddr = CADDR(dStack.getTop(1));

			if (length > 1 && getDataChar( caddr ) == '-') {
				dStack.setTop(static_cast<Cell>(length - 1));
				dStack.setTop(1, CELL(caddr + 1));
				parseUnsignedNumber(numericBase);
				dStack.setTop(2, static_cast<Cell>(-static_cast<SCell>(dStack.getTop(2))));
			}
			else {
				parseUnsignedNumber(numericBase);
			}
		}
		void parseHashNumber() {
			REQUIRE_DSTACK_DEPTH(3, ">#NUM");
			auto length = SIZE_T(dStack.getTop());
			auto caddr = CADDR(dStack.getTop(1));

			if (length > 1 && getDataChar(caddr) == '#') {
				dStack.setTop(static_cast<Cell>(length - 1));
				dStack.setTop(1, CELL(caddr + 1));
				parseSignedNumber(10);
			}
			else 			if (length > 1 && getDataChar(caddr) == '$') {
				dStack.setTop(static_cast<Cell>(length - 1));
				dStack.setTop(1, CELL(caddr + 1));
				parseSignedNumber(16);
			}
			else if (length > 1 && getDataChar(caddr) == '%') {
				dStack.setTop(static_cast<Cell>(length - 1));
				dStack.setTop(1, CELL(caddr + 1));
				parseSignedNumber(2);
			}
			else if (length == 3 && getDataChar(caddr) == '\'' && getDataChar(caddr+2) == '\'') {
				dStack.setTop(static_cast<Cell>(length - 3));
				dStack.setTop(1, CELL(caddr + 3));
				dStack.setTop(2, CELL(getDataChar(caddr+1)));

			}

			else {
				parseSignedNumber(getNumericBase());
			}

		}
		unsigned long parseUnsignedNumber(const std::string valueStr, int skip, size_t &i, int numericBase) {

			auto length = valueStr.length();
			auto caddr = valueStr.c_str();
			auto value = 0;

			 i = std::size_t(skip);
			while (i < length) {
				auto c = valueStr.at(i);
				if (isValidDigit(c, numericBase)) {
					auto n = digitValue(c);
					value = value * numericBase + n;
					++i;
				}
				else {
					break;
				}
			}
			return value;
		}

		long parseSignedNumber(const std::string value, size_t &rest, int numericBase) {
			auto length = value.length();			
			if (length > rest+1 && value[rest] == '-') {
				return -static_cast<long>(parseUnsignedNumber(value,rest+1,rest, numericBase));
			}
			else {
				return parseUnsignedNumber(value,0, rest, numericBase);
			}
		}

		bool interpretNumbers(const std::string &value){
			static std::regex regexInt{ "^[\\-]?\\d+$" };
			static std::regex regexDouble{ "^\\d+\\.$" };
			static std::regex regexNegativeDouble{ "^\\-\\d+\\.$" };
			static std::regex regexDoubleNot10{ "^[\\dA-Za-z]+\\.$" };
			static std::regex regexNegativeDoubleNot10{ "^\\-[\\dA-Za-z]+\\.$" };
			static std::regex regexInt10{ "^#([\\-]?\\d+)$" };
			static std::regex regexDouble10{ "^#([\\-]?\\d+)\\.$" };
			static std::regex regexDouble16{ "^\\$([\\-]?[\\dA-Fa-f]+)\\.$" };
			static std::regex regexDouble2{ "^%([\\-]?[01]+)\\.$" };
			static std::regex regexInt16{ "^\\$([\\-]?[0-9A-Fa-f]+)$" };
			static std::regex regexInt2{ "^%([\\-]?[01]+)$" };
			static std::regex regexIntChar{ "^\'(.{1,1})\'$" };

			// regex float ^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$
			// regex double ^[-+]?\d+\.$
			// regex int ^[-+]?\d+$
			int base = getNumericBase();
			// consider it is int
			if(0){
			auto size=value.size();
			if(base==10 && size>0 && value[size-1]>='0' && value[size-1]<='9'){
				size_t last{};
				long long int number = std::stoll(value,&last,base);
				if (last == size && number<=INT32_MAX && number>=INT32_MIN ) {
					Cell cnumber(number);
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(cnumber);
					}
					else {
						push(cnumber);
					}
					return true;
				}
			}
			}
			std::smatch m1{};
			bool found{};
			if (base == 10){
				found = regex_search(value, m1, regexDouble);
				if (found){
					unsigned long long int number = std::stoll(value);
					DCell numberD(number);
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.lo);
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.hi);

					}
					else {
						push(numberD.data_.Cells.lo);
						push(numberD.data_.Cells.hi);
					}
					return true;
				}
				found = regex_search(value, m1, regexNegativeDouble);
				if (found){
					long long int number = std::stoll(value);
					SDCell numberD(number);
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.lo);
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.hi);
					}
					else {
						push(numberD.data_.Cells.lo);
						push(numberD.data_.Cells.hi);
					}
					return true;
				}
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT
				std::regex regexFloat{ "^[-+]{0,1}[0-9]*\\.{0,1}[0-9]*[eE]([-+]?[0-9]+)?$" };
				found = regex_search(value, m1, regexFloat);
				if (found){
					FCell number = std::stod(value);
					if (getIsCompiling()) {
						data(CELL(doFLiteralXt));
						dataFloat(number);
					}
					else {
						fStack.push(number);
					}
					return true;
				}
#endif
			}
			found = regex_search(value, m1, regexDoubleNot10);
			if (found) {
				size_t ptr{};
				unsigned long long int number = std::stoll(value,&ptr, base);
				DCell numberD(number);
				if (getIsCompiling()) {
					data(CELL(doLiteralXt));
					data(numberD.data_.Cells.lo);
					data(CELL(doLiteralXt));
					data(numberD.data_.Cells.hi);

				}
				else {
					push(numberD.data_.Cells.lo);
					push(numberD.data_.Cells.hi);
				}
				return true;
			}
			found = regex_search(value, m1, regexNegativeDoubleNot10);
			if (found) {
				size_t ptr{};
				long long int number = std::stoll(value,&ptr,base);
				SDCell numberD(number);
				if (getIsCompiling()) {
					data(CELL(doLiteralXt));
					data(numberD.data_.Cells.lo);
					data(CELL(doLiteralXt));
					data(numberD.data_.Cells.hi);
				}
				else {
					push(numberD.data_.Cells.lo);
					push(numberD.data_.Cells.hi);
				}
				return true;
			}
				found = regex_search(value, m1, regexInt10);
				if (found){
					size_t ptr; int number;
					try{
						number = std::stoi(m1[1], &ptr, 10);
					}
					catch (...){
						ptr = ptr;
					}
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(number);
					}
					else {
						push(number);
					}
					return true;
				}
				found = regex_search(value, m1, regexDouble10);
				if (found) {
					size_t ptr; long long number;
					try {
						number = std::stoll(m1[1], &ptr, 10);
					}
					catch (...) {
						ptr = ptr;
					}
					SDCell numberD(number);
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.lo);
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.hi);
					}
					else {
						push(numberD.data_.Cells.lo);
						push(numberD.data_.Cells.hi);
					}
					return true;
				}
				found = regex_search(value, m1, regexDouble16);
				if (found) {
					size_t ptr; long long number;
					try {
						number = std::stoll(m1[1], &ptr, 16);
					}
					catch (...) {
						ptr = ptr;
					}
					SDCell numberD(number);
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.lo);
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.hi);
					}
					else {
						push(numberD.data_.Cells.lo);
						push(numberD.data_.Cells.hi);
					}
					return true;
				}
				found = regex_search(value, m1, regexDouble2);
				if (found) {
					size_t ptr; long long number;
					try {
						number = std::stoll(m1[1], &ptr, 2);
					}
					catch (...) {
						ptr = ptr;
					}
					SDCell numberD(number);
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.lo);
						data(CELL(doLiteralXt));
						data(numberD.data_.Cells.hi);
					}
					else {
						push(numberD.data_.Cells.lo);
						push(numberD.data_.Cells.hi);
					}
					return true;
				}
				found = regex_search(value, m1, regexInt16);
				if (found){
					size_t ptr; int number;
					try{
						number = std::stoi(m1[1], &ptr, 16);
					}
					catch (...){
						ptr = ptr;
					}
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(number);
					}
					else {
						push(number);
					}
					return true;
				}
				found = regex_search(value, m1, regexInt2);
				if (found){
					size_t ptr; int number;
					try{
						number = std::stoi(m1[1], &ptr, 2);
					}
					catch (...){
						ptr = ptr;
					}
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(number);
					}
					else {
						push(number);
					}
					return true;
				}
			
				found = regex_search(value, m1, regexIntChar);
				if (found){
					unsigned number;
					try{
						std::string num{ m1[1] };
						number = num[0];
					}
					catch (...){
						size_t ptr = 0;
					}
					if (getIsCompiling()) {
						data(CELL(doLiteralXt));
						data(number);
					}
					else {
						push(number);
					}
					return true;
				}

				found = true;
				if (found){
					size_t  ptr2{};
					int number2=parseSignedNumber(value, ptr2, base);
					if (ptr2 == value.length()){
						if (getIsCompiling()) {
							data(CELL(doLiteralXt));
							data(number2);
						}
						else {
							push(number2);
						}
						return true;
					}
					number2 = number2;
				}
			return false;
		}
		enum InterpretStates { InterpretSource=0, InterpretWords=1 };
		InterpretStates InterpretState = InterpretSource;
		void interpret() {
			Xt xt{};
			while (true){
				if (InterpretState == InterpretSource){
					if (getSourceBufferRemain() >0 ) {
						bl();
						word();
						find();
						auto found = static_cast<int>(dStack.getTop()); pop();
						if (found) {
							xt = XT(dStack.getTop()); pop();
							if (getIsCompiling() && !getDefinition(xt)->isImmediate()) {
								data(CELL(xt));
							}
							else {
								InterpretState = InterpretWords;
							}
						}
						else {
							// Try to parse it as a number.
							count();
							auto length = SIZE_T(dStack.getTop()); pop();
							auto caddr = CADDR(dStack.getTop()); pop();
							if (length > 0) {
								std::string currentWord{};
								moveFromDataSpace(currentWord, caddr, length);
								if (!interpretNumbers(currentWord)){
									throwMessage(std::string("unrecognized word: ") + currentWord, errorUndefinedWord);
								}
							}
							else {
								return;
							}
						}
					}
					else {
						refill();
						auto refillFlag = dStack.getTop(); pop();
						if (!refillFlag){
							auto qty = savedInput.size();
							if (qty > 0){
								RestoreInput();
								if (InterpretState == InterpretWords){
									if (next_command != 0){
										xt = getDataCell(next_command);
#ifdef _DEBUG
										if (xt >= definitions.size()){
											throwMessage("wrong XT", errorInvalidAddress);
										}
#endif
										next_command += sizeof(next_command);
									}
								}
							}
							else {
								break;
							}
						}
					}
				}
				if (InterpretState == InterpretWords){
					//if (next_command != 0){
						auto defn = getDefinition(xt);
						while (defn->code == &Forth::execute) {
							REQUIRE_DSTACK_DEPTH(1, "EXECUTE");
							xt = dStack.getTop(); dStack.pop();
							defn = getDefinition(xt);
						}
#ifdef FORTHSCRIPTCPP_ENABLE_INTERNALDEBUGGER
						if (stepfunction){
							if (currentstep-- == 0){
								stepfunction(this, xt, next_command);
								currentstep = maxstep;
							}
						}
#endif
						getDefinition(xt)->executeDefinition(*this);
						if (next_command == 0){
							InterpretState = InterpretSource;
						}
						else {
							xt = getDataCell(next_command);
#ifdef _DEBUG
							if (xt >= definitions.size()){
								throwMessage("wrong XT", errorInvalidAddress);
							}
#endif							
							next_command += sizeof(next_command);
						}
					//}
					if (next_command == 0){
						InterpretState = InterpretSource;
					}
				}
			}
		}

		/****

		`EVALUATE` can be used to invoke `INTERPRET` with a string as the source
		buffer.

		****/
		struct structSavedInput {
			InterpretStates interpretState_{};
			size_t inputBufferStringsCurrent_{};
			Cell SourceBufferOffset_{};
			Cell SourceBufferAddress_{};
			Cell SourceBufferSize_{}; 
			Cell next_{};
			Cell SourceDashId_{};
			enum InputBufferSourceSavedByEnum { fromFile, fromEvaluate } InputBufferSourceSavedBy_{ fromFile };
			std::vector<std::string> inputBufferStrings_{};
		};
		std::vector<structSavedInput> savedInput{};
		void SaveInput(structSavedInput::InputBufferSourceSavedByEnum source){
			struct structSavedInput save{};
			save.inputBufferStringsCurrent_ = inputBufferStringsCurrent;
			std::swap(save.inputBufferStrings_ ,inputBufferStrings);  // consider std::swap here
			inputBufferStringsCurrent = 0;
			getSourceVariables(save.SourceBufferAddress_, save.SourceBufferSize_, save.SourceBufferOffset_);
			save.InputBufferSourceSavedBy_ = source;
			save.SourceDashId_ = getSourceId();
			save.interpretState_ = InterpretState;
			save.next_ = next_command;
			next_command = 0;
			savedInput.push_back(save);
		}
		void SetInput(const std::string &str){
			std::string tokens{ "\n" }; // @bug \n\r ?
			inputBufferStrings.clear();
			split_str(str, inputBufferStrings, tokens);
			inputBufferStringsCurrent = 0;
			setSourceBuffer(inputBufferStrings.at(inputBufferStringsCurrent));
		}
		size_t RestoreInput(){
			auto size = savedInput.size();
			if (size > 0){
				struct structSavedInput &save{ savedInput.at(size-1)};
				if (save.inputBufferStrings_.size()>0){
					std::swap(save.inputBufferStrings_, inputBufferStrings);
					inputBufferStringsCurrent = save.inputBufferStringsCurrent_;
					setSourceVariables(save.SourceBufferAddress_, save.SourceBufferSize_, save.SourceBufferOffset_);
					setSourceId(save.SourceDashId_);
					InterpretState = save.interpretState_;
					next_command = save.next_;
				}
				savedInput.pop_back();
			}
			return savedInput.size();
		}
		// EVALUATE ( i*x c-addr u -- j*x )
		// This function redefined in initialization string (check the reason).
		void evaluate() {
			REQUIRE_DSTACK_DEPTH(2, "EVALUATE");
			SaveInput(structSavedInput::fromEvaluate);
			auto length = static_cast<std::size_t>(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop()); pop();
			setSourceId(-1);
			setSourceVariables(caddr, length, 0);
			// @bug delete this commented lines, obsolete
			//auto &sourceBufferVirtual = VirtualMemory.at(vmSegmentSourceBuffer).segment;
			//sourceBufferVirtual.resize(length); 
			//moveFromDataSpace(&sourceBufferVirtual[0], caddr, length);
			//setSourceBufferOffset(0);
			InterpretState = InterpretSource;
			interpret();
			RestoreInput();
		}
		// EVALUATE ( i*x c-addr u -- j*x )
		void evaluateStart() {
			REQUIRE_DSTACK_DEPTH(2, "EVALUATE");
			SaveInput(structSavedInput::fromEvaluate);
			auto length = static_cast<std::size_t>(dStack.getTop()); pop();
			auto caddr = CADDR(dStack.getTop()); pop();
			// @bug delete this lines after debug
			//std::string buffer{};
			//moveFromDataSpace(buffer, caddr, length);
			//SetInput(buffer);
			setSourceVariables(caddr, length, 0);
			InterpretState = InterpretSource;
		}
		// EVALUATESTOP (  --  ) not standard word
		void evaluateStop() {
			; // RestoreInput() is called in interpret(), when current source is ended
		}

		// S>D  
		void StoD(){
			REQUIRE_DSTACK_DEPTH(1, "S>D");
			REQUIRE_DSTACK_AVAILABLE(1, "S>D");
			SCell top = dStack.getTop();
			if (top < 0)
				dStack.push(-1);
			else
				dStack.push(0);
		}
		// D>S ( d -- n )  
		void DtoS(){
			REQUIRE_DSTACK_DEPTH(2, "D>S");
			DCell d(dStack.getTop(1), dStack.getTop());
			dStack.pop();
			SCell n = d.data_.SDcells;
			dStack.setTop(0,n);
		}
		// M*
		void MultD(){
			REQUIRE_DSTACK_DEPTH(2, "M*");
			SDCell n1{ static_cast<SCell>(dStack.getTop()) };
			SDCell n2{ static_cast<SCell>(dStack.getTop(1)) };
			n1.data_.Dcells *= n2.data_.Dcells;
			dStack.setTop(1, n1.data_.Cells.lo);
			dStack.setTop(0, n1.data_.Cells.hi);
		}
		// UM*
		void MultUD(){
			REQUIRE_DSTACK_DEPTH(2, "UM*");
			DCell n1{ dStack.getTop() };
			DCell n2 { dStack.getTop(1) };
			n1.data_.Dcells *= n2.data_.Dcells;
			dStack.setTop(1, n1.data_.Cells.lo);
			dStack.setTop(0, n1.data_.Cells.hi);
		}
		// FM/MOD  (d1 n1 -- n2 n3)
		//Divide d1 by n1, giving the floored quotient n3 and the remainder n2.
		// Input and output stack arguments are signed.
		void FMdivideMOD(){
			REQUIRE_DSTACK_DEPTH(3, "FM/MOD");
			SDCell n1{ static_cast<SCell>(dStack.getTop()) }; dStack.pop();
			SDCell d1(dStack.getTop(1),dStack.getTop());
			RUNTIME_ERROR_IF(n1.data_.Dcells == 0, "FM/MOD: zero divisor", errorDivisionByZero);
			SDCell quotent{ d1.data_.Dcells / n1.data_.Dcells }; // n3
			SDCell reminder{ d1.data_.Dcells % n1.data_.Dcells }; // n2
			if (((reminder.data_.Dcells > 0) && (n1.data_.Dcells < 0)) || 
				((reminder.data_.Dcells < 0) && (n1.data_.Dcells > 0))){
				reminder.data_.Dcells += n1.data_.Dcells;
				quotent.data_.Dcells--;
			}
			dStack.setTop(0, quotent.data_.Cells.lo);
			dStack.setTop(1, reminder.data_.Cells.lo);
		}
		// FM/MOD  (d1 n1 -- n2 n3)
		//simmetric Divide d1 by n1, giving the simmetric quotient n3 and the remainder n2.
		// Input and output stack arguments are signed.
		void SMdivideREM(){
			REQUIRE_DSTACK_DEPTH(3, "SM/REM");
			SDCell n1{ static_cast<SCell>(dStack.getTop()) }; dStack.pop();
			RUNTIME_ERROR_IF(n1.data_.Cells.lo == 0, "SM/REM: zero divisor", errorDivisionByZero);
			SDCell d1(dStack.getTop(1),dStack.getTop());
			SDCell quotent(d1.data_.Dcells / n1.data_.Dcells); // n3
			SDCell reminder(d1.data_.Dcells % n1.data_.Dcells); // n2
			dStack.setTop(0, quotent.data_.Cells.lo);
			dStack.setTop(1, reminder.data_.Cells.lo);
		}
		// UM/MOD  (d1 n1 -- n2 n3)
		//( ud u1 -- u2 u3 )
		//Divide ud by u1, giving the quotient u3 and the remainder u2.All values and arithmetic are unsigned.
		void UMdivideMOD(){
			REQUIRE_DSTACK_DEPTH(3, "UM/MOD");
			DCell n1(dStack.getTop()); dStack.pop();
			RUNTIME_ERROR_IF(n1.data_.Cells.lo == 0, "UM/MOD: zero divisor", errorDivisionByZero);
			DCell d1(dStack.getTop(1),dStack.getTop(0));
			DCell quotent(d1.data_.Dcells / n1.data_.Dcells); // n3
			dStack.setTop(0, quotent.data_.Cells.lo);
			DCell reminder(d1.data_.Dcells % n1.data_.Dcells); // n2
			dStack.setTop(1, reminder.data_.Cells.lo);
		}
		// */ ( n1 n2 n3 -- n4 )
		// Multiply n1 by n2 producing the intermediate double - cell result d.
		// Divide d by n3 giving the single - cell quotient n4.
		void MultDivide(){
			REQUIRE_DSTACK_DEPTH(3, "*/");
			SDCell n3(static_cast<SCell>(dStack.getTop())); dStack.pop();
			SDCell n2(static_cast<SCell>(dStack.getTop())); dStack.pop();
			RUNTIME_ERROR_IF(n3.data_.Cells.lo == 0, "*/: zero divisor", errorDivisionByZero);
			SDCell n1(static_cast<SCell>(dStack.getTop()));
			SDCell n4( (n1.data_.Dcells*n2.data_.Dcells) / n3.data_.Dcells);
			dStack.setTop(0, n4.data_.Cells.lo);
		}
		// */MOD ( n1 n2 n3 -- n4 n5 )
		// Multiply n1 by n2 producing the intermediate double-cell result d. 
		// Divide d by n3 producing the single-cell remainder n4 and the single-cell quotient n5.
		void MultDivideMod(){
			REQUIRE_DSTACK_DEPTH(3, "*/MOD");
			SDCell n3(static_cast<SCell>(dStack.getTop())); dStack.pop();
			SDCell n2(static_cast<SCell>(dStack.getTop())); 
			RUNTIME_ERROR_IF(n3.data_.Cells.lo == 0, "*/: zero divisor", errorDivisionByZero);
			SDCell n1(static_cast<SCell>( dStack.getTop(1)));
			SDCell quotent( (n1.data_.Dcells*n2.data_.Dcells) / n3.data_.Dcells );
			SDCell reminder( (n1.data_.Dcells*n2.data_.Dcells) % n3.data_.Dcells );
			dStack.setTop(1, reminder.data_.Cells.lo);
			dStack.setTop(0, quotent.data_.Cells.lo);
		}
		// DNEGATE ( d -- d ) 
		void dnegate(){
			REQUIRE_DSTACK_DEPTH(2, "DNEGATE");
			DCell d1(dStack.getTop(1), dStack.getTop());
			d1.data_.Dcells = -d1.data_.Dcells;
			dStack.setTop(1, d1.data_.Cells.lo);
			dStack.setTop(0, d1.data_.Cells.hi);
		}
		// D+ ( d1 | ud1 d2 | ud2 -- d3 | ud3 )
		void dplus(){
			REQUIRE_DSTACK_DEPTH(4, "D+");
			DCell d1(dStack.getTop(1), dStack.getTop());
			DCell d2(dStack.getTop(3), dStack.getTop(2));
			dStack.pop();dStack.pop();
			d1.data_.Dcells = d1.data_.Dcells+d2.data_.Dcells;
			dStack.setTop(1, d1.data_.Cells.lo);
			dStack.setTop(0, d1.data_.Cells.hi);
		}
		// M+ ( d1 | ud1 n -- d2 | ud2 ) Add n to d1 | ud1, giving the sum d2 | ud2.
		void mplus(){
			REQUIRE_DSTACK_DEPTH(3, "M+");
			SCell n(dStack.getTop());
			DCell d1(dStack.getTop(2), dStack.getTop(1));
			dStack.pop();
			d1.data_.Dcells = d1.data_.Dcells+n;
			dStack.setTop(1, d1.data_.Cells.lo);
			dStack.setTop(0, d1.data_.Cells.hi);
		}
		/// M*/ ( d1 n1 +n2 -- d2 ) Multiply d1 by n1 producing the triple-cell intermediate result t. 
		// Divide t by +n2 giving the double-cell quotient d2. An ambiguous condition exists if +n2 is zero or negative, 
		// or the quotient lies outside of the range of a double-precision signed integer.
		void mstarslash(){
			bool changeSign=false;
			REQUIRE_DSTACK_DEPTH(4, "M*/");
			SCell n2(dStack.getTop());
			SCell n1(dStack.getTop(1));
			DCell d1(dStack.getTop(3), dStack.getTop(2));
			dStack.pop();dStack.pop();
			// make all operations with positive unsigned values
			// sign apply later
			if(n2 != 0){
			if(n2 < 0){
				n2=-n2; changeSign=!changeSign;
			}
			if(n1 < 0){
				n1=-n1; changeSign=!changeSign;
			}
			if(d1.data_.SDcells < 0){
				d1.data_.SDcells=-d1.data_.SDcells; changeSign=!changeSign;
			}
			DCell l(d1.data_.Cells.lo,0);
			DCell h(d1.data_.Cells.hi,0);
			l.data_.Dcells *= static_cast<Cell>(n1);
			h.data_.Dcells *= static_cast<Cell>(n1);
			DCell ldiv =  l.data_.Dcells / static_cast<Cell>(n2); // first add
			DCell hdiv =  h.data_.Dcells / static_cast<Cell>(n2);
			DCell second(0,hdiv.data_.Cells.lo);				  // second add
			Cell lmod =  l.data_.Dcells % static_cast<Cell>(n2);
			Cell hmod =  h.data_.Dcells % static_cast<Cell>(n2);
			DCell third(lmod,hmod);
			third= third.data_.Dcells / static_cast<Cell>(n2); // third add

			d1.data_.Dcells = ldiv.data_.Dcells+second.data_.Dcells+third.data_.Dcells;
			if(changeSign){
				d1.data_.SDcells=-d1.data_.SDcells;
			}

			dStack.setTop(1, d1.data_.Cells.lo);
			dStack.setTop(0, d1.data_.Cells.hi);
			} else {
				/// throw exception
			}
		}

		// D-  ( d1 | ud1 d2 | ud2 -- d3 | ud3 )  d1 - d2
		void dminus(){
			REQUIRE_DSTACK_DEPTH(4, "D-");
			DCell d2(dStack.getTop(1), dStack.getTop());
			DCell d1(dStack.getTop(3), dStack.getTop(2));
			d1.data_.Dcells = d1.data_.Dcells-d2.data_.Dcells;
			dStack.pop();dStack.pop();
			dStack.setTop(1, d1.data_.Cells.lo);
			dStack.setTop(0, d1.data_.Cells.hi);
		}
		// D<  ( d1 d2 -- flag )  d1 < d2
		void dless(){
			REQUIRE_DSTACK_DEPTH(4, "D<");
			DCell d2(dStack.getTop(1), dStack.getTop());
			DCell d1(dStack.getTop(3), dStack.getTop(2));
			auto result = d1.data_.SDcells<d2.data_.SDcells;
			dStack.pop();dStack.pop();dStack.pop();
			dStack.setTop(0, result? True : False);
		}
		// DU<  ( ud1 ud2 -- flag )  ud1 < ud2
		void duless(){
			REQUIRE_DSTACK_DEPTH(4, "DU<");
			DCell d2(dStack.getTop(1), dStack.getTop());
			DCell d1(dStack.getTop(3), dStack.getTop(2));
			auto result = d1.data_.Dcells<d2.data_.Dcells;
			dStack.pop();dStack.pop();dStack.pop();
			dStack.setTop(0, result? True : False);
		}
		// D0=  ( xd -- flag ) flag is true if and only if xd is equal to zero.
		void dzeroequal(){
			REQUIRE_DSTACK_DEPTH(2, "D0=");
			DCell d1(dStack.getTop(1), dStack.getTop());
			auto result = d1.data_.SDcells==0;
			dStack.pop();
			dStack.setTop(0, result? True : False);
		}
		// D0<  ( d -- flag ) flag is true if and only if d is less than zero.
		void dzeroless(){
			REQUIRE_DSTACK_DEPTH(2, "D0<");
			DCell d1(dStack.getTop(1), dStack.getTop());
			auto result = d1.data_.SDcells<0;
			dStack.pop();
			dStack.setTop(0, result? True : False);
		}
		// D=  ( xd1 xd2 -- flag ) flag is true if and only if xd1 is bit-for-bit the same as xd2.
		void dequal(){
			REQUIRE_DSTACK_DEPTH(4, "D=");
			DCell d2(dStack.getTop(1), dStack.getTop());
			DCell d1(dStack.getTop(3), dStack.getTop(2));
			auto result = d1.data_.SDcells==d2.data_.SDcells;
			dStack.pop();dStack.pop();dStack.pop();
			dStack.setTop(0, result? True : False);
		}
		// D2* ( xd1 -- xd2 ) xd2 is the result of shifting xd1 one bit toward the most-significant bit, filling the vacated least-significant bit with zero.
		void d2star(){
			REQUIRE_DSTACK_DEPTH(2, "D2*");
			DCell d1(dStack.getTop(1), dStack.getTop());
			d1.data_.Dcells <<= 1;
			dStack.setTop(1, d1.data_.Cells.lo);
			dStack.setTop(0, d1.data_.Cells.hi);
		}
		// D2/  ( xd1 -- xd2 ) xd2 is the result of shifting xd1 one bit toward the least-significant bit, leaving the most-significant bit unchanged.


		void d2slash(){
			REQUIRE_DSTACK_DEPTH(2, "D2/");
			DCell d1(dStack.getTop(1), dStack.getTop());
			d1.data_.SDcells >>= 1;
			dStack.setTop(1, d1.data_.Cells.lo);
			dStack.setTop(0, d1.data_.Cells.hi);
		}
		// DMAX  ( d1 | ud1 d2 | ud2 -- d3 | ud3 )  d1 - d2
		void dmax(){
			REQUIRE_DSTACK_DEPTH(4, "DMAX");
			DCell d2(dStack.getTop(1), dStack.getTop());
			DCell d1(dStack.getTop(3), dStack.getTop(2));
			dStack.pop();dStack.pop();
			if(d2.data_.SDcells>d1.data_.SDcells){
			 dStack.setTop(1, d2.data_.Cells.lo);
			 dStack.setTop(0, d2.data_.Cells.hi);
			}
		}
		/// DMIN
		void dmin(){
			REQUIRE_DSTACK_DEPTH(4, "DMIN");
			DCell d2(dStack.getTop(1), dStack.getTop());
			DCell d1(dStack.getTop(3), dStack.getTop(2));
			dStack.pop();dStack.pop();
			if(d2.data_.SDcells<d1.data_.SDcells){
			 dStack.setTop(1, d2.data_.Cells.lo);
			 dStack.setTop(0, d2.data_.Cells.hi);
			}
		}
		/// <summary>
		///  2ROT ( x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2 )
		/// Rotate the top three cell pairs on the stack bringing cell pair x1 x2 to the top of the stack.
		/// </summary>
		void d2rot() {
			REQUIRE_DSTACK_DEPTH(6, "2ROT");
			Cell x6(dStack.getTop());
			Cell x5(dStack.getTop(1));
			Cell x4(dStack.getTop(2));
			Cell x3(dStack.getTop(3));
			Cell x2(dStack.getTop(4));
			Cell x1(dStack.getTop(5));
			dStack.setTop(0, x2);
			dStack.setTop(1, x1);
			dStack.setTop(2, x6);
			dStack.setTop(3, x5);
			dStack.setTop(4, x4);
			dStack.setTop(5, x3);
		}
		/*****
		The pictured input buffer 
		
		*****/
		// >NUMBER       ud1 c-addr1 u1 � ud2 c-addr2 u2 
		void toNumber(){
			REQUIRE_DSTACK_DEPTH(4, ">NUMBER");
			Cell u1 = dStack.getTop();
			Cell caddr1 = dStack.getTop(1);
			DCell ud1(dStack.getTop(3),dStack.getTop(2));
			DCell base( getNumericBase() );
			while (u1 > 0){
				Char ch = getDataChar(caddr1);

				size_t position{} ;
				if (isValidDigit(ch)){
					position = digitValue(ch);
				} else {
					break;
				}
				--u1; ++caddr1;
				ud1.data_.Dcells = ud1.data_.Dcells * base.data_.Dcells + position;
			}
			dStack.setTop(3, ud1.data_.Cells.lo);
			dStack.setTop(2, ud1.data_.Cells.hi);
			dStack.setTop(1, caddr1);
			dStack.setTop(0, u1);


		}
		void PicturedInputBufferStart(){
			picturedInputBuffer.resize(0);
		}
		void PicturedInputBufferHash(){
			REQUIRE_DSTACK_DEPTH(2, "#");
			DCell d1(dStack.getTop(1),dStack.getTop(0));
			DCell base( getNumericBase() );
			RUNTIME_ERROR_IF(base.data_.Dcells == 0, "#: zero numeric base", errorUnsupportedOperation);
			RUNTIME_ERROR_IF(base.data_.Dcells > 36, "#: numeric base > 36", errorUnsupportedOperation);
			picturedInputBuffer.push_back(static_cast<Char>(digitsCodes.at(d1.data_.Dcells % base.data_.Dcells)));
			DCell d2( d1.data_.Dcells / base.data_.Dcells );
			dStack.setTop(1, d2.data_.Cells.lo);
			dStack.setTop(0, d2.data_.Cells.hi);
		}
		void PicturedInputBufferHashS(){
			REQUIRE_DSTACK_DEPTH(2, "#S");
			do {
				PicturedInputBufferHash();
			} while ((dStack.getTop() != 0) || (dStack.getTop(1) != 0));
		}
		void PicturedInputBufferHold(){
			REQUIRE_DSTACK_DEPTH(1, "HOLD");
			Char ch = static_cast<Char>(dStack.getTop()); dStack.pop();
			picturedInputBuffer.push_back(static_cast<Char>(ch));
		}
		void PicturedInputBufferSign(){
			REQUIRE_DSTACK_DEPTH(1, "SIGN");
			SCell sign = static_cast<SCell>(dStack.getTop()); dStack.pop();
			if (sign < 0){
				picturedInputBuffer.push_back(static_cast<Char>('-'));
			}
		}
		// #>     (  xd � addr u )
		void PicturedInputBufferEnd(){
			REQUIRE_DSTACK_DEPTH(2, "#>");
			auto dataInDataSpace = PutStringToEndOfDataSpace(std::string(picturedInputBuffer.rbegin(), picturedInputBuffer.rend()));
			dStack.setTop(1,CELL(dataInDataSpace));
			dStack.setTop(picturedInputBuffer.size());

		}

		/*****
		 Exceptions
		
		******/
		// Exception stack
		ForthStack<Cell> catchStack{};

		Cell exceptionHandler{};
		void exceptionsThrow(){
			SCell exceptionNumber = static_cast<SCell>(dStack.getTop()); 
			if (exceptionNumber != 0){
				if (exceptionHandler >= 4 && catchStack.stackDepth() >= 4){
					while (catchStack.stackDepth() > exceptionHandler) catchStack.pop();
					exceptionHandler = catchStack.getTop(); catchStack.pop();
					auto returnDepth = catchStack.getTop(); catchStack.pop();
					while (returnStack.stackDepth() > returnDepth) returnStack.pop();
					auto rDepth = catchStack.getTop(); catchStack.pop();
					while (rStack.stackDepth() > rDepth) rStack.pop();
					while (rStack.stackDepth() < rDepth) rStack.push(0);
					auto dDepth = catchStack.getTop(); catchStack.pop();
					while (dStack.stackDepth() > dDepth) dStack.pop();
					while (dStack.stackDepth() < dDepth) dStack.push(0);

					next_command = returnStack.getTop(); returnStack.pop();
					dStack.setTop(exceptionNumber);
				}
				else {
					if (exceptionNumber == -1){
						abort();
					}
					abort();
				}
			}
			else {
				dStack.pop();
			}
		}
		void exceptionsCatchBefore(){
			catchStack.push(dStack.stackDepth());
			catchStack.push(rStack.stackDepth());
			catchStack.push(returnStack.stackDepth());

			catchStack.push(exceptionHandler);
			exceptionHandler = catchStack.stackDepth();
		}
		void exceptionsCatchAfter(){
			exceptionHandler = catchStack.getTop(); catchStack.pop();
			catchStack.pop();
			catchStack.pop();
			catchStack.pop();
			dStack.push(0);
		}

		/****

		`QUIT` is the top-level outer interpreter loop. It calls `REFILL` to read a
		line, `INTERPRET` to parse and execute that line, then `PROMPT` and repeat
		until there is no more input.

		There is an std::exception handler for `AbortException` that prints an error
		message, resets the stacks, and continues.

		If end-of-input occurs, then it exits the loop and calls `CR` and `BYE`.

		If `QUIT` is called from a word called by `QUIT`, control returns to the
		top-level loop.

		****/

		// PROMPT ( -- )
		//
		// Not a standard word.
		//
		// Displays "ok" prompt if in interpretation mode.
		void prompt() {
			if (!getIsCompiling()) {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
				std_cout << "  ok";
#endif
				cr();
			}
		}

		bool alreadyRunning = false;
		// QUIT ( -- )
		void quit() {

			if (alreadyRunning)
				abort();
			alreadyRunning = true;

			rStack.resetStack();
			setIsCompiling( False );

			for (; !isBye;) {
				try {
					refill();
					auto refilled = dStack.getTop(); pop();
					if (!refilled) // end-of-input
						break;

					interpret();
				}
				catch (const AbortException& abortEx) {
					std::string msg(abortEx.what());
					if (msg.length() > 0) {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
						switch (writeToTarget) {
						case ToString:
							std_cout << "<<< " << msg << " >>>" << std::endl;
							break;
						case ToStdCout:
							std::cout << "<<< " << msg << " >>>" << std::endl;
							std::cout.flush();
							break;
						default:
							break;
						}
#endif
					}
					dStack.resetStack();
					rStack.resetStack();
					controlStackIf_Begin.resetStack();
					controlStackLoops.resetStack();
					setIsCompiling( False );
				}
				prompt();
			}
			cr();
		}


#ifdef FORTHSCRIPTCPP_ENABLE_FILE
#include "ForthScriptCppFile.h"
#endif
#ifdef FORTHSCRIPTCPP_ENABLE_MEMORY
#include "ForthScriptCppMemory.h"
#endif
		/****

		Initialization
		--------------

		In `initializeDefinitions()`, we set up the initial contents of the dictionary.
		This is the Forth kernel that Forth code can use to implement the rest of a
		working system.

		****/
		enum {
			wrongCommandXt =0,
			doLiteralXt = 1,
			setDoesXt = 2,
			exitXt = 3,
			endOfDefinitionXt = 4,
			branchXt = 5,
			zbranchXt = 6,
			toR_Xt = 7,
			rFrom_Xt = 8,
			DropDropXt = 9,
			DupDupXt = 10,
			Plus1Xt = 11,
			EqualsXt = 12,
			rotXt = 13,
			plusXt = 14,
			swapXt = 15,
			plusLoopCheckXt = 16 ,
			noopXt = 17,
			doFLiteralXt = 18


		};
		void definePrimitives() {
			typedef struct {
				const char* name;
				Code code;
				bool immediate;
			} CodeWord;

			CodeWord codeWords[] = {
				// name             code
				// ------------------------------
				{ "", nullptr, true }, // empty difinition (zero index - not found)
				{ "(lit)", &Forth::doLiteral, false },
				{ "(does)", &Forth::setDoes, false },
				{ "exit", &Forth::exit, false },
				{ "(;)", &Forth::endOfDefinition, false },
				{ "(branch)", &Forth::branch, false },
				{ "(zbranch)", &Forth::zbranch, false },
				{ ">r", &Forth::toR, false },
				{ "r>", &Forth::rFrom, false },
				{ "2drop",&Forth::drop2, false },
				{ "2dup", &Forth::dup2, false },
				{ "1+", &Forth::plus1, false },
				{ "=", &Forth::equals, false },
				{ "rot", &Forth::rot, false },
				{ "+", &Forth::plus, false },  // CORE
				{ "swap", &Forth::swap, false },
				{ "", &Forth::loops_plusloop_check, false },
				{ "noop", &Forth::noop, false }, // 17
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT
				{ "(flit)", &Forth::doFLiteral, false }, // 18
#else
				{ "noop", &Forth::noop, false }, // 18
#endif
				{ ";", &Forth::semicolon, true }, // CORE
				{ "!", &Forth::store, false },  // CORE
				{ "*", &Forth::star, false }, // CORE
				{ "-", &Forth::minus, false }, // CORE
				{ ".", &Forth::dot, false },   // CORE
				{ ".r", &Forth::dotR, false },  // CORE EXT
				{ ".rs", &Forth::dotRS, false }, // not standard
				{ ".s", &Forth::dotS, false }, // TOOLS
				{ "/", &Forth::slash, false },  // CORE
				{ "/mod", &Forth::slashMod, false },  // CORE
				{ ":", &Forth::colon, false },  // CORE
				{ ":noname", &Forth::noname, false },  // CORE EXT
				{ "<", &Forth::lessThan, false },  // CORE
				{ ">", &Forth::greaterThan, false },  // CORE
				{ ">body", &Forth::toBody, false }, // CORE
				{ ">in", &Forth::toIn, false }, // CORE
				{ "@", &Forth::fetch, false },  // CORE
				{ "abort", &Forth::abort, false },  // CORE
				{ "abort-message", &Forth::abortMessage, false },  // not standard
				{ "accept", &Forth::accept, false },  // CORE
				{ "align", &Forth::align, false },  // CORE
				{ "aligned", &Forth::aligned, false },  // CORE
				{ "allot", &Forth::allot, false },  // CORE
				{ "and", &Forth::bitwiseAnd, false },  // CORE
				{ "base", &Forth::base, false },  // CORE
				{ "bl", &Forth::bl, false },  // CORE
				{ "bye", &Forth::bye, false }, // TOOLS EXT
				{ "c!", &Forth::cstore, false }, // CORE
				{ "c@", &Forth::cfetch, false },  // CORE
				{ "cells", &Forth::cells, false },  // CORE
				{ "cmove", &Forth::cMove, false }, // STRING
				{ "cmove>", &Forth::cMoveUp, false }, // STRING
				{ "compare", &Forth::compare, false }, // STRING
				{ "count", &Forth::count, false },  // CORE
				{ "cr", &Forth::cr, false },  // CORE
				{ "create", &Forth::create, false },  // CORE
				{ "depth", &Forth::depth, false },  // CORE
				{ "does>", &Forth::does, true },  // CORE
				{ "drop", &Forth::drop, false },  // CORE
				{ "dup", &Forth::dup, false }, // CORE
				{ "emit", &Forth::emit, false },  // CORE
				{ "evaluate", &Forth::evaluate, false },  // CORE
				{ "evaluatestart", &Forth::evaluateStart, false },  // not standard word
				{ "evaluatestop", &Forth::evaluateStop, false },  // not standard word
				{ "execute", &Forth::execute, false },  // CORE
				{ "fill", &Forth::fill, false },  // CORE
				{ "find", &Forth::find, false }, // CORE
				{ "here", &Forth::here, false }, // CORE
				{ "immediate", &Forth::immediate, false }, // CORE // immediate is not immediate command, it can be compiled
				//{ "interpret", &Forth::interpret, false }, // not standard word
				{ "key", &Forth::key, false }, // CORE
				{ "key?", &Forth::key_question, false }, // FACILITY
				{ "latest", &Forth::latest, false }, // not standard word
				{ "lshift", &Forth::lshift, false },  // CORE
				{ "ms", &Forth::ms, false }, // FACILITY EXT
				{ "or", &Forth::bitwiseOr, false }, // CORE
				{ "pad", &Forth::pad, false }, // CORE EXT
				{ "parse", &Forth::parse, false }, // CORE EXT
				{ "pick", &Forth::pick, false }, // CORE EXT
				//{ "prompt", &Forth::prompt, false }, // not standard word
				{ "quit", &Forth::quit, false }, // CORE
				{ "r@", &Forth::rFetch, false }, // CORE
				{ "refill", &Forth::refill, false }, // CORE EXT
				{ "roll", &Forth::roll, false },  // CORE EXT
				{ "rshift", &Forth::rshift, false },  // CORE
				{ "2/", &Forth::rshiftBy1, false }, // CORE
				{ "see", &Forth::see, false }, // TOOLS
				{ "source", &Forth::source, false }, // CORE
				{ "source-id", &Forth::sourcedashid, false }, // CORE
				{ "state", &Forth::state, false }, // CORE
				{ "time&date", &Forth::timeAndDate, false }, // FACILITY EXT
				{ "ms@", &Forth::ms_at, false }, // not standard word - milliseconds since Forth start execution
				{ "type", &Forth::type, false },  // CORE
				{ "u<", &Forth::uLessThan, false },  // CORE
				{ "u>", &Forth::uGreaterThan, false }, // CORE EXT
				{ "u.", &Forth::uDot, false },  // CORE
				{ "unused", &Forth::unused, false }, // CORE EXT
				{ "utctime&date", &Forth::utcTimeAndDate, false }, // not standard word
				{ "word", &Forth::word, false }, // CORE
				{ "words", &Forth::words, false }, // TOOLS
				{ "xt>name", &Forth::xtToName, false }, // not standard word
				{ "xor", &Forth::bitwiseXor, false }, // CORE
				{ "do", &Forth::loops_do, true }, // CORE
				{ "?do", &Forth::loops_question_do, true }, // CORE EXT
				{ "loop", &Forth::loops_loop, true },  // CORE
				{ "+loop", &Forth::loops_plusloop, true }, // CORE
				{ "leave", &Forth::loops_leave, true }, // CORE
				{ "i", &Forth::loops_i, false }, // CORE
				{ "j", &Forth::loops_j, false }, // CORE
				{ "k", &Forth::loops_k, false }, // not standard word
				{ "unloop", &Forth::loops_unloop, false }, // CORE
				{ "if", &Forth::ifthenelse_if, true }, // CORE
				{ "then", &Forth::ifthenelse_then, true }, // CORE
				{ "else", &Forth::ifthenelse_else, true }, // CORE
				{ "ahead", &Forth::ifthenelse_ahead, true }, // TOOLS EXT
				{ "begin", &Forth::begin_etc_begin, true }, // CORE
				{ "again", &Forth::begin_etc_again, true }, // CORE
				{ "until", &Forth::begin_etc_until, true }, // CORE
				{ "while", &Forth::begin_etc_while, true }, // CORE
				{ "repeat", &Forth::begin_etc_repeat, true }, // CORE
				{ "[ELSE2]", &Forth::bracketElse, true }, // TOOLS
				{ "S>D", &Forth::StoD, false }, // CORE
				{ "D>S", &Forth::DtoS, false }, // DOUBLE
				{ "M*", &Forth::MultD, false }, // CORE
				{ "UM*", &Forth::MultUD, false }, // CORE
				{ "UM/MOD", &Forth::UMdivideMOD, false }, // CORE
				{ "FM/MOD", &Forth::FMdivideMOD, false }, // CORE
				{ "SM/REM", &Forth::SMdivideREM, false }, // CORE
				{ "<#", &Forth::PicturedInputBufferStart, false }, // CORE
				{ "#", &Forth::PicturedInputBufferHash, false }, // CORE
				{ "#S", &Forth::PicturedInputBufferHashS, false }, // CORE
				{ "HOLD", &Forth::PicturedInputBufferHold, false }, // CORE
				{ "SIGN", &Forth::PicturedInputBufferSign, false }, // CORE
				{ "#>", &Forth::PicturedInputBufferEnd, false }, // CORE
				{ "*/", &Forth::MultDivide, false }, // CORE
				{ "*/MOD", &Forth::MultDivideMod, false }, // CORE
				{ "DNEGATE", &Forth::dnegate, false }, // DOUBLE-NUMBER
				{ "D+", &Forth::dplus, false }, // DOUBLE-NUMBER
				{ "M+", &Forth::mplus, false }, // DOUBLE-NUMBER
				{ "M*/", &Forth::mstarslash, false }, // DOUBLE-NUMBER
				{ "D-", &Forth::dminus, false }, // DOUBLE-NUMBER
				{ "D<", &Forth::dless, false }, // DOUBLE-NUMBER
				{ "DU<", &Forth::duless, false }, // DOUBLE-NUMBER
				{ "D0=", &Forth::dzeroequal, false }, // DOUBLE-NUMBER
				{ "D0<", &Forth::dzeroless, false }, // DOUBLE-NUMBER
				{ "D2*", &Forth::d2star, false }, // DOUBLE-NUMBER
				{ "D2/", &Forth::d2slash, false }, // DOUBLE-NUMBER
				{ "D=",  &Forth::dequal, false }, // DOUBLE-NUMBER
				{ "DMAX",  &Forth::dmax, false }, // DOUBLE-NUMBER
				{ "DMIN",  &Forth::dmin, false }, // DOUBLE-NUMBER
				{ "2ROT",  &Forth::d2rot, false }, // DOUBLE-NUMBER
				{ ">NUMBER", &Forth::toNumber, false }, // CORE
				{ "-TRAILING", &Forth::dashtrailing, false }, // STRINGS
				{ "SEARCH", &Forth::search, false }, // STRINGS
				// { "SCAN", &Forth::scan, false }, // STRINGS-NON STANDARD WORD for substitute
				{ "CATCHBEFORE", &Forth::exceptionsCatchBefore, false }, // not standerd word
				{ "CATCHAFTER", &Forth::exceptionsCatchAfter, false }, // not standerd word
				{ "THROW", &Forth::exceptionsThrow, false }, // not standerd word
				{ "ENVIRONMENT?", &Forth::environmentquestion, false }, // not standard word
				{ "TRACEON", &Forth::setTraceOn, false }, // not standard word
				{ "TRACEOFF", &Forth::setTraceOff, false }, // not standard word
				{ "SEARCH-WORDLIST", &Forth::searchdashwordlist, false }, // SEARCH
				{ "CS-PICK", &Forth::csdashpick, false }, // TOOLS
				{ "CS-ROLL", &Forth::csdashroll, false }, // TOOLS
				{ "NR>", &Forth::nrmore, false }, // TOOLS
				{ "N>R", &Forth::nmorer, false }, // TOOLS
				{ "SYNONYM", &Forth::synonym, false }, // TOOLS
				

				
				

#ifdef FORTHSCRIPTCPP_ENABLE_MEMORY
				{ "resize", &Forth::memResize , false }, //MEMORY
				{ "allocate", &Forth::memAllocate , false }, // MEMORY
				{ "free", &Forth::memFree , false }, // MEMORY
#endif
#ifdef FORTHSCRIPTCPP_ENABLE_FILE
				{ "bin", &Forth::bin, false },
				{ "close-file", &Forth::closeFile, false },
				{ "create-file", &Forth::createFile, false },
				{ "delete-file", &Forth::deleteFile, false },
				{ "flush-file", &Forth::flushFile, false },
				{ "include-file", &Forth::includeFile, false },
				{ "open-file", &Forth::openFile, false },
				{ "r/o", &Forth::readOnly, false },
				{ "r/w", &Forth::readWrite, false },
				{ "read-char", &Forth::readChar, false },
				{ "read-file", &Forth::readFile, false },
				{ "read-line", &Forth::readLine, false },
				{ "rename-file", &Forth::renameFile, false },
				{ "w/o", &Forth::writeOnly, false },
				{ "write-char", &Forth::writeChar, false },
				{ "write-file", &Forth::writeFile, false },
				{ "write-line", &Forth::writeLine, false },
				{ "file-position", &Forth::filePosition, false },

#endif
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT
				{ "D>F", &Forth::f_dtof, false },
				{ "F>D", &Forth::f_ftod, false },
				{ "FDUP", &Forth::f_dup, false },
				{ "F+", &Forth::f_plus, false },
				{ "FDEPTH", &Forth::f_depth, false },
				{ "F-", &Forth::f_minus, false },
				{ "F/", &Forth::f_slash, false },
				{ "F*", &Forth::f_star, false },
				{ "F<0", &Forth::f_less0, false },
				{ "F0=", &Forth::f_equals0, false },
				{ "FDROP", &Forth::f_drop, false },
				{ "F<", &Forth::f_less, false },
				{ "FALIGN", &Forth::f_align, false },
				{ "FALIGNED", &Forth::f_aligned, false },
				{ "FLOAT+", &Forth::f_floatplus, false },
				{ "FLOATS", &Forth::f_floats, false },
				{ "FLOOR", &Forth::f_floor, false },
				{ "FMAX", &Forth::f_fmax, false },
				{ "FMIN", &Forth::f_fmin, false },
				{ "FROUND", &Forth::f_fround, false },
				{ "FTRUNC", &Forth::f_ftrunc, false },
				{ "FNEGATE", &Forth::f_fnegate, false },
				{ "FROT", &Forth::f_rot, false },
				{ "FPICK", &Forth::f_pick, false },
				{ "FSWAP", &Forth::f_swap, false },
				{ "F!", &Forth::f_store, false },
				{ "F@", &Forth::f_fetch, false },
				{ "F~", &Forth::f_ftilda, false },
				{ ">FLOAT", &Forth::toFloat, false },
				{ "REPRESENT", &Forth::represent, false },
#endif
#ifdef FORTHSCRIPTCPP_ENABLE_FLOAT_EXT
				{ "F>S", &Forth::f_fmores, false },
				{ "S>F", &Forth::f_smoref, false },
				{ "FABS", &Forth::f_fabs, false },
				{ "FSIN", &Forth::f_fsin, false },
				{ "FCOS", &Forth::f_fcos, false },
				{ "FTAN", &Forth::f_ftan, false },
				{ "FASIN", &Forth::f_fasin, false },
				{ "FACOS", &Forth::f_facos, false },
				{ "FATAN", &Forth::f_fatan, false },
				{ "FATAN2", &Forth::f_fatan2, false },
				{ "FSINCOS", &Forth::f_fsincos, false },
				{ "FSQRT", &Forth::f_fsqrt, false },
				{ "FEXP", &Forth::f_fexp, false },
				{ "FEXPM1", &Forth::f_fexpm1, false },
				{ "FLN", &Forth::f_fln, false },
				{ "FLOG", &Forth::f_flog, false },
				{ "FLNP1", &Forth::f_flnp1, false },
				{ "F**", &Forth::f_fpower, false },
				{ "FALOG", &Forth::f_falog, false },
				{ "FASINH", &Forth::f_fasinh, false },
				{ "FACOSH", &Forth::f_facosh, false },
				{ "FSINH", &Forth::f_fsinh, false },
				{ "FCOSH", &Forth::f_fcosh, false },
				{ "FTANH", &Forth::f_ftanh, false },
				{ "PRECISION", &Forth::f_precision, false },
				{ "SET-PRECISION", &Forth::f_setprecision, false },
				{ "F.", &Forth::f_fdot, false },
				{ "FS.", &Forth::f_fsdot, false },
				{ "FE.", &Forth::f_fedot, false },
#endif
			};

			int numWords = sizeof(codeWords) / sizeof(CodeWord);

			for (int i = 0; i < numWords; i++)
			{
				CodeWord &w = codeWords[i];
				definePrimitive(w.name, w.code,w.immediate);
			}

#ifdef _DEBUG
			if (doLiteralXt != findDefinition("(lit)")) 
				throwMessage("Can't find (lit) in kernel dictionary",errorUndefinedWord);
			if (setDoesXt != findDefinition("(does)")) 
				throwMessage("Can't find (does) in kernel dictionary", errorUndefinedWord);
			if (exitXt != findDefinition("exit")) 
				throwMessage("Can't find EXIT in kernel dictionary", errorUndefinedWord);
			if (endOfDefinitionXt != findDefinition("(;)")) 
				throwMessage("Can't find (;) in kernel dictionary", errorUndefinedWord);
#endif
		}
		
		void moveIntoDataSpace(Cell dst, const char *src, size_t count){
			while (count--){
				dataSpaceSet(dst++, *src++);
			}
		}
		void moveFromDataSpace(unsigned char *dst, Cell src, size_t count){
			while (count--){
				*dst++ = dataSpaceAt(src++);
			}
		}
		void moveFromDataSpace(std::string &dst, Cell src, size_t count){
			dst.resize(0);
			dst.reserve(count);
			while (count--){
				dst.push_back( dataSpaceAt(src++) );
			}
		}

		size_t PutStringToEndOfDataSpace(const std::string &data){
			auto size = data.size();
			if (size + getDataPointer() + 2 >= VirtualMemory[vmSegmentDataSpace].end  ){
				throwMessage("Not enough data space memory or input buffer memory", errorInvalidAddress);
			}
			auto offset = VirtualMemory[vmSegmentDataSpace].end - size - 2;
			moveIntoDataSpace(offset, data.data(), size);
			return offset;
		}
		/* @bug delete this member 
		size_t PutStringToStartOfDataSpace(const std::string &data){
			auto size = data.size();
			auto offset = getDataPointer(); 
			incDataPointer(  size );
			alignDataPointer();
			moveIntoDataSpace(offset, data.data(), size+1);
			return offset;
		}
		*/
		void defineForthWords() {
				auto line = forthDefinitions;
				auto length = std::strlen(line);
				SetInput( line );
				interpret();
		}

		void initializeDefinitions() {
			definitions.clear();
			definePrimitives();
			defineForthWords();
		}

		enum {
			vmSegmentVariables=0,
			vmSegmentSourceBufferRefill=1,
			vmSegmentDataSpace=2
		} vmSegments;
		void forthscriptcpp_reset() {
			size_t sourceBufferSize = 0x010000;
			executionStarted = std::chrono::duration_cast< std::chrono::milliseconds >(
				std::chrono::system_clock::now().time_since_epoch() );
			dStack.resize(FORTHSCRIPTCPP_DSTACK_COUNT);
			dStack.resetStack();
			rStack.resize(FORTHSCRIPTCPP_RSTACK_COUNT);
			rStack.resetStack();
			Cell start = 0, end = 0x10000; // VarOffsetSourceBuffer;
			VirtualMemorySegment variables{ start, end, std::vector<Char>(end) };
			if (VirtualMemory.size() == vmSegmentVariables) VirtualMemory.push_back(variables);
			start = end;
			end += sourceBufferSize;
			VirtualMemorySegment sourceBuffer{ start, end, std::vector<Char>(sourceBufferSize) };
			if (VirtualMemory.size() == vmSegmentSourceBufferRefill) VirtualMemory.push_back(sourceBuffer);
			// set SOURCE, SOURCE size, >IN
			setSourceVariables(start, 0, 0); // size if 0, source is empty
			start = end;
			end += FORTHSCRIPTCPP_DATASPACE_SIZE;
			VirtualMemorySegment dataSpace{ start, end , std::vector<Char>(FORTHSCRIPTCPP_DATASPACE_SIZE) };
			if (VirtualMemory.size() == vmSegmentDataSpace) VirtualMemory.push_back(dataSpace);
			setDataPointer(start);
			VirtualMemoryFreeSegment = end;
			setSourceId(0); // SOURCE-ID=0: user input 
			setNumericBase(10); // BASE = 10
			setIsCompiling(False);
			setDataCell(VarOffsetBlkAddress, Cell(0)); // set BLK=0
			// return stack init;
			returnStack.resize(100);
			
			initializeDefinitions();
		}
		/******
		Read from virtual memory
		******/
		struct VirtualMemorySegment { Cell start,end; std::vector<Char> segment; };
		std::vector<VirtualMemorySegment> VirtualMemory{};
		Cell VirtualMemoryFreeSegment{};
		Char dataSpaceAt(Cell index){
			for (auto it = VirtualMemory.begin(); it != VirtualMemory.end(); ++it){
				if ((*it).start <= index && (*it).end > index){
					return (*it).segment.at(index - (*it).start);
				}
			}
			return Char(0); // all memory is filled by 0x00
			throwMessage("Access outside dataspace",errorInvalidAddress);
			throw;
		}
		void dataSpaceSet(Cell index,Char value) {
			for (auto it = VirtualMemory.begin(); it != VirtualMemory.end(); ++it) {
				if ((*it).start <= index && (*it).end > index) {
					(*it).segment.at(index - (*it).start) = value;
				}
			}
			// ignore writes outside of allocated memory (it is ROM)
			//throwMessage("Access outside dataspace", errorInvalidAddress);
			//throw;
		}

	public:
		Forth(){
			forthscriptcpp_reset();
		};
		/// embeding interface

		// push value to stack
		void forth_push(Cell value){ 
			push(value); 
		} 
		// return the depth of the stack
		size_t forth_depth(){ 
			return dStack.stackDepth(); 
		} 
		// return true, if stack is available for 'num' elements
		bool forth_available(size_t num){
			return dStack.availableStack(num);
		}

		// pop num elements from stack
		void forth_pop(size_t num)
		{ 
			while(num-- > 0) pop(); 
		} 
		// read element from stack
		Cell forth_tocell(size_t index)
		{ 
			return dStack.getTop(index); 
		};
		private:
			std::map<Xt, void(*)(Forth* ptr)> cfunctions{};
		void execute_cfunction(){
			auto num = executingWord;
			auto finded = cfunctions.find(num) ;
			if (finded != cfunctions.end()){
				auto function = finded->second;
				if(function)
					function(this);
			}
		}
		public:

		void forth_setcfunction(void(*function)(Forth* ptr) , const std::string &name){
			definePrimitive(name.c_str(), &Forth::execute_cfunction, false);
			auto last = lastDefinition().numberInVector;
			cfunctions[last] = function;
		}
#ifdef FORTHSCRIPTCPP_ENABLE_INTERNALDEBUGGER
		private:
		void(*stepfunction)(Forth* ptr,Cell xt,Cell cell){};
		size_t maxstep{}, currentstep{};
		public:
		void forth_setstepfunction(void(*function)(Forth* ptr, Cell xt, Cell cell),size_t newmaxstep){
			stepfunction = function;
			maxstep = newmaxstep;
			currentstep = 0;
		}
#endif

		void forth_readmemory(CAddr address, Cell size, std::vector<char> &buffer){
			buffer.resize(0);
			buffer.reserve(size);
			while (size--){
				buffer.push_back(dataSpaceAt(address++));
			}
		}
		void forth_writememory(CAddr address, Cell size, std::vector<char> &buffer){
			size_t offset = 0;
			if (size > buffer.size()) size = buffer.size();
			while (size--){
				dataSpaceSet(address++, buffer.at(offset++));
			}
		}

			std::vector<std::string> inputBufferStrings{};
			size_t inputBufferStringsCurrent{};
			public:

		void ExecuteString(const std::string &commands){
			SetInput(commands);
			for (inputBufferStringsCurrent = 0; inputBufferStringsCurrent<inputBufferStrings.size(); 
				++inputBufferStringsCurrent){
				setSourceBuffer(inputBufferStrings.at(inputBufferStringsCurrent));
				setSourceBufferOffset(0);
				interpret();
			}
		}
		std::string ExecutionOutput() const {
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			return std_cout.str();
#else
			return "";
#endif
		}
		void ExecutionOutputReset(){
#ifndef FORTHSCRIPTCPP_DISABLE_OUTPUT
			std::stringstream().swap(std_cout);
#endif
		}
		void SetExecutionInputBuffer(const std::string &input){
			std_cin.str(input);
		}

		bool isExecutionInputBufferEmpty() const{
			return std_cin.str().empty();
		}

		void ExecutionInputBufferReset(){
			std::stringstream().swap(std_cin);
		}
		void ExecutionErrorBufferReset(){
#ifndef			FORTHSCRIPTCPP_DISABLE_OUTPUT
			std::stringstream().swap(std_cerr);
#endif
		}
		bool isExecutionErrorBufferEmpty() const{
#ifndef			FORTHSCRIPTCPP_DISABLE_OUTPUT
			return std_cerr.str().empty();
#endif
		}
			private:
		bool traceCalls{};
		void setTraceOn(){
			setTraceCalls(true);
		}
		void setTraceOff(){
			setTraceCalls(false);
		}
		public:
		bool isTraceCalls(){
			return traceCalls;	
		}
		void setTraceCalls(bool value){
			traceCalls=value;
		}
	}; // end of class
} // end  namespace

#endif
