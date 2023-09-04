variable core-words-check 0 core-words-check ! 
variable core-words-missing 0 core-words-missing ! 
: word-check-core bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr core-words-missing 
@ 1+ core-words-missing ! then core-words-check @ 1+ core-words-check ! ; 


word-check-core !
word-check-core #
word-check-core #>
word-check-core #S
word-check-core '
word-check-core (
word-check-core *
word-check-core */
word-check-core */MOD
word-check-core +
word-check-core +!
word-check-core +LOOP
word-check-core ,
word-check-core -
word-check-core .
word-check-core ."
word-check-core /
word-check-core /MOD
word-check-core 0<
word-check-core 0=
word-check-core 1+
word-check-core 1-
word-check-core 2!
word-check-core 2*
word-check-core 2/
word-check-core 2@
word-check-core 2DROP
word-check-core 2DUP
word-check-core 2OVER
word-check-core 2SWAP
word-check-core :
word-check-core ;
word-check-core <
word-check-core <#
word-check-core =
word-check-core >
word-check-core >BODY
word-check-core >IN
word-check-core >NUMBER
word-check-core >R
word-check-core ?DUP
word-check-core @
word-check-core ABORT
word-check-core ABORT"
word-check-core ABS
word-check-core ACCEPT
word-check-core ALIGN
word-check-core ALIGNED
word-check-core ALLOT
word-check-core AND
word-check-core BASE
word-check-core BEGIN
word-check-core BL
word-check-core C!
word-check-core C,
word-check-core C@
word-check-core CELL+
word-check-core CELLS
word-check-core CHAR
word-check-core CHAR+
word-check-core CHARS
word-check-core CONSTANT
word-check-core COUNT
word-check-core CR
word-check-core CREATE
word-check-core DECIMAL
word-check-core DEPTH
word-check-core DO
word-check-core DOES>
word-check-core DROP
word-check-core DUP
word-check-core ELSE
word-check-core EMIT
word-check-core ENVIRONMENT?
word-check-core EVALUATE
word-check-core EXECUTE
word-check-core EXIT
word-check-core FILL
word-check-core FIND
word-check-core FM/MOD
word-check-core HERE
word-check-core HOLD
word-check-core I
word-check-core IF
word-check-core IMMEDIATE
word-check-core INVERT
word-check-core J
word-check-core KEY
word-check-core LEAVE
word-check-core LITERAL
word-check-core LOOP
word-check-core LSHIFT
word-check-core M*
word-check-core MAX
word-check-core MIN
word-check-core MOD
word-check-core MOVE
word-check-core NEGATE
word-check-core OR
word-check-core OVER
word-check-core POSTPONE
word-check-core QUIT
word-check-core R>
word-check-core R@
word-check-core RECURSE
word-check-core REPEAT
word-check-core ROT
word-check-core RSHIFT
word-check-core S"
word-check-core S>D
word-check-core SIGN
word-check-core SM/REM
word-check-core SOURCE
word-check-core SPACE
word-check-core SPACES
word-check-core STATE
word-check-core SWAP
word-check-core THEN
word-check-core TYPE
word-check-core U.
word-check-core U<
word-check-core UM*
word-check-core UM/MOD
word-check-core UNLOOP
word-check-core UNTIL
word-check-core VARIABLE
word-check-core WHILE
word-check-core WORD
word-check-core XOR
word-check-core [
word-check-core [']
word-check-core [CHAR]
word-check-core ]


S"   core words total " type core-words-check @ . S"   core words missing " type core-words-missing @ . CR 

variable coreext-words-check 0 coreext-words-check ! 
variable coreext-words-missing 0 coreext-words-missing ! 
: word-check-coreext bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr coreext-words-missing 
@ 1+ coreext-words-missing ! then coreext-words-check @ 1+ coreext-words-check ! ; 
word-check-coreext .(
word-check-coreext .R
word-check-coreext 0<>
word-check-coreext 0>
word-check-coreext 2>R
word-check-coreext 2R>
word-check-coreext 2R@
word-check-coreext :NONAME
word-check-coreext <>
word-check-coreext ?DO
word-check-coreext ACTION-OF
word-check-coreext AGAIN
word-check-coreext BUFFER:
word-check-coreext C"
word-check-coreext CASE
word-check-coreext COMPILE,
word-check-coreext DEFER
word-check-coreext DEFER!
word-check-coreext DEFER@
word-check-coreext ENDCASE
word-check-coreext ENDOF
word-check-coreext ERASE
word-check-coreext FALSE
word-check-coreext HEX
word-check-coreext HOLDS
word-check-coreext IS
word-check-coreext MARKER
word-check-coreext NIP
word-check-coreext OF
word-check-coreext PAD
word-check-coreext PARSE
word-check-coreext PARSE-NAME
word-check-coreext PICK
word-check-coreext REFILL
word-check-coreext RESTORE-INPUT
word-check-coreext ROLL
word-check-coreext S\"
word-check-coreext SAVE-INPUT
word-check-coreext SOURCE-ID
word-check-coreext TO
word-check-coreext TRUE
word-check-coreext TUCK
word-check-coreext U.R
word-check-coreext U>
word-check-coreext UNUSED
word-check-coreext VALUE
word-check-coreext WITHIN
word-check-coreext [COMPILE]
word-check-coreext \

S"   coreext words total " type coreext-words-check @ . S"   coreext words missing " type coreext-words-missing @ . CR 

variable exceptions-words-check 0 exceptions-words-check ! 
variable exceptions-words-missing 0 exceptions-words-missing ! 
: word-check-exceptions bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr exceptions-words-missing 
@ 1+ exceptions-words-missing ! then exceptions-words-check @ 1+ exceptions-words-check ! ; 



word-check-exceptions ABORT
word-check-exceptions ABORT"
word-check-exceptions CATCH
word-check-exceptions THROW

S"   exceptions words total " type exceptions-words-check @ . S"   exceptions words missing " type exceptions-words-missing @ . CR 

variable float-words-check 0 float-words-check ! 
variable float-words-missing 0 float-words-missing ! 
: word-check-float bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr float-words-missing 
@ 1+ float-words-missing ! then float-words-check @ 1+ float-words-check ! ; 
word-check-float DFALIGN
word-check-float DFALIGNED
word-check-float DFFIELD:
word-check-float DF@
word-check-float DFLOAT+
word-check-float DFLOATS
word-check-float DF!
word-check-float D>F
word-check-float FABS
word-check-float FACOS
word-check-float FACOSH
word-check-float FALIGN
word-check-float FALIGNED
word-check-float FALOG
word-check-float FASIN
word-check-float FASINH
word-check-float FATAN
word-check-float FATANH
word-check-float FATAN2
word-check-float FCONSTANT
word-check-float FCOS
word-check-float FCOSH
word-check-float FDEPTH
word-check-float FDROP
word-check-float FDUP
word-check-float F/
word-check-float FEXP
word-check-float FEXPM1
word-check-float FE.
word-check-float FFIELD:
word-check-float F@
word-check-float FLITERAL
word-check-float FLN
word-check-float FLNP1
word-check-float FLOAT+
word-check-float FLOATS
word-check-float FLOG
word-check-float FLOOR
word-check-float FMAX
word-check-float FMIN
word-check-float F-
word-check-float FNEGATE
word-check-float FOVER
word-check-float F+
word-check-float FROT
word-check-float FROUND
word-check-float FSIN
word-check-float FSINCOS
word-check-float FSINH
word-check-float FSQRT
word-check-float FSWAP
word-check-float FS.
word-check-float F!
word-check-float FTAN
word-check-float FTANH
word-check-float FTRUNC
word-check-float F*
word-check-float F**
word-check-float FVALUE
word-check-float FVARIABLE
word-check-float F0=
word-check-float F0<
word-check-float F.
word-check-float F<
word-check-float F~
word-check-float F>D
word-check-float F>S
word-check-float PRECISION
word-check-float REPRESENT
word-check-float SET-PRECISION
word-check-float SFALIGN
word-check-float SFALIGNED
word-check-float SFFIELD:
word-check-float SF@
word-check-float SFLOAT+
word-check-float SFLOATS
word-check-float SF!
word-check-float S>F
word-check-float >FLOAT
 
S"   float words total " type float-words-check @ . S"   float words missing " type float-words-missing @ . CR 

 variable tools-words-check 0 tools-words-check ! 
variable tools-words-missing 0 tools-words-missing ! 
: word-check-tools bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr tools-words-missing 
@ 1+ tools-words-missing ! then tools-words-check @ 1+ tools-words-check ! ; 
word-check-tools AHEAD
word-check-tools ASSEMBLER
word-check-tools BYE
word-check-tools [DEFINED]
word-check-tools [ELSE]
word-check-tools [IF]
word-check-tools [THEN]
word-check-tools [UNDEFINED]
word-check-tools CODE
word-check-tools CS-PICK
word-check-tools CS-ROLL
word-check-tools DUMP
word-check-tools .S
word-check-tools EDITOR
word-check-tools FORGET
word-check-tools NAME>COMPILE
word-check-tools NAME>INTERPRET
word-check-tools NAME>STRING
word-check-tools NR>
word-check-tools N>R
word-check-tools SEE
word-check-tools STATE
word-check-tools SYNONYM
word-check-tools ;CODE
word-check-tools TRAVERSE-WORDLIST
word-check-tools WORDS
word-check-tools ?

S"   tools words total " type tools-words-check @ . S"   tools words missing " type tools-words-missing @ . CR 

variable xchar-words-check 0 xchar-words-check ! 
variable xchar-words-missing 0 xchar-words-missing ! 
: word-check-xchar bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr xchar-words-missing 
@ 1+ xchar-words-missing ! then xchar-words-check @ 1+ xchar-words-check ! ; 

word-check-xchar [CHAR]
word-check-xchar CHAR
word-check-xchar EKEY>XCHAR
word-check-xchar -TRAILING-GARBAGE
word-check-xchar PARSE
word-check-xchar +X/STRING
word-check-xchar X-SIZE
word-check-xchar X-WIDTH
word-check-xchar XC-SIZE
word-check-xchar XC-WIDTH
word-check-xchar XC,
word-check-xchar XC@+
word-check-xchar XCHAR-
word-check-xchar XCHAR+
word-check-xchar XC!+
word-check-xchar XC!+?
word-check-xchar XEMIT
word-check-xchar XHOLD
word-check-xchar XKEY
word-check-xchar XKEY?
word-check-xchar X\STRING-

S"   xchar words total " type xchar-words-check @ . S"   xchar words missing " type xchar-words-missing @ . CR 

variable block-words-check 0 block-words-check ! 
variable block-words-missing 0 block-words-missing ! 
: word-check-block bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr block-words-missing 
@ 1+ block-words-missing ! then block-words-check @ 1+ block-words-check ! ; 

word-check-block BLK
word-check-block BLOCK
word-check-block BUFFER
word-check-block EMPTY-BUFFERS
word-check-block EVALUATE
word-check-block FLUSH
word-check-block LIST
word-check-block LOAD
word-check-block REFILL
word-check-block SAVE-BUFFERS
word-check-block SCR
word-check-block THRU
word-check-block UPDATE
word-check-block \

S"   block words total " type block-words-check @ . S"   block words missing " type block-words-missing @ . CR 

variable double-words-check 0 double-words-check ! 
variable double-words-missing 0 double-words-missing ! 
: word-check-double bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr double-words-missing 
@ 1+ double-words-missing ! then double-words-check @ 1+ double-words-check ! ; 


word-check-double DABS
word-check-double D.R
word-check-double D=
word-check-double DMAX
word-check-double DMIN
word-check-double D-
word-check-double DNEGATE
word-check-double D+
word-check-double D2/
word-check-double D2*
word-check-double DU<
word-check-double D0=
word-check-double D0<
word-check-double D.
word-check-double D<
word-check-double D>S
word-check-double M+
word-check-double M*/
word-check-double 2CONSTANT
word-check-double 2LITERAL
word-check-double 2ROT
word-check-double 2VALUE
word-check-double 2VARIABLE

S"   double words total " type double-words-check @ . S"   double words missing " type double-words-missing @ . CR 

variable facility-words-check 0 facility-words-check ! 
variable facility-words-missing 0 facility-words-missing ! 
: word-check-facility bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr facility-words-missing 
@ 1+ facility-words-missing ! then facility-words-check @ 1+ facility-words-check ! ; 
word-check-facility AT-XY
word-check-facility BEGIN-STRUCTURE
word-check-facility CFIELD:
word-check-facility EKEY
word-check-facility EKEY?
word-check-facility EKEY>CHAR
word-check-facility EKEY>FKEY
word-check-facility EMIT?
word-check-facility END-STRUCTURE
word-check-facility FIELD:
word-check-facility K-ALT-MASK
word-check-facility K-CTRL-MASK
word-check-facility K-DELETE
word-check-facility K-DOWN
word-check-facility K-END
word-check-facility K-F3
word-check-facility K-F4
word-check-facility K-F5
word-check-facility K-F6
word-check-facility K-F7
word-check-facility K-F8
word-check-facility K-F9
word-check-facility K-F1
word-check-facility K-F11
word-check-facility K-F12
word-check-facility K-F10
word-check-facility K-F2
word-check-facility K-HOME
word-check-facility K-INSERT
word-check-facility K-LEFT
word-check-facility K-NEXT
word-check-facility K-PRIOR
word-check-facility K-RIGHT
word-check-facility K-SHIFT-MASK
word-check-facility K-UP
word-check-facility KEY?
word-check-facility MS
word-check-facility PAGE
word-check-facility +FIELD
word-check-facility TIME&DATE

S"   facility words total " type facility-words-check @ . S"   facility words missing " type facility-words-missing @ . CR 

variable file-words-check 0 file-words-check ! 
variable file-words-missing 0 file-words-missing ! 
: word-check-file bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr file-words-missing 
@ 1+ file-words-missing ! then file-words-check @ 1+ file-words-check ! ; 
word-check-file BIN
word-check-file CLOSE-FILE
word-check-file CREATE-FILE
word-check-file DELETE-FILE
word-check-file FILE-POSITION
word-check-file FILE-SIZE
word-check-file FILE-STATUS
word-check-file FLUSH-FILE
word-check-file INCLUDE-FILE
word-check-file INCLUDE
word-check-file INCLUDED
word-check-file OPEN-FILE
word-check-file R/O
word-check-file R/W
word-check-file READ-FILE
word-check-file READ-LINE
word-check-file REFILL
word-check-file RENAME-FILE
word-check-file REPOSITION-FILE
word-check-file REQUIRE
word-check-file REQUIRED
word-check-file RESIZE-FILE
word-check-file SOURCE-ID
word-check-file S\"
word-check-file S"
word-check-file W/O
word-check-file WRITE-FILE
word-check-file WRITE-LINE
word-check-file (

S"   file words total " type file-words-check @ . S"   file words missing " type file-words-missing @ . CR 

variable locals-words-check 0 locals-words-check ! 
variable locals-words-missing 0 locals-words-missing ! 
: word-check-locals bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr locals-words-missing 
@ 1+ locals-words-missing ! then locals-words-check @ 1+ locals-words-check ! ; 
word-check-locals (LOCAL)
word-check-locals LOCALS|
word-check-locals {:

S"   locals words total " type locals-words-check @ . S"   locals words missing " type locals-words-missing @ . CR 

variable memory-words-check 0 memory-words-check ! 
variable memory-words-missing 0 memory-words-missing ! 
: word-check-memory bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr memory-words-missing 
@ 1+ memory-words-missing ! then memory-words-check @ 1+ memory-words-check ! ; 
word-check-memory ALLOCATE
word-check-memory FREE
word-check-memory RESIZE

S"   memory words total " type memory-words-check @ . S"   memory words missing " type memory-words-missing @ . CR 

variable search-words-check 0 search-words-check ! 
variable search-words-missing 0 search-words-missing ! 
: word-check-search bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr search-words-missing 
@ 1+ search-words-missing ! then search-words-check @ 1+ search-words-check ! ; 
word-check-search ALSO
word-check-search DEFINITIONS
word-check-search FIND
word-check-search FORTH-WORDLIST
word-check-search FORTH
word-check-search GET-CURRENT
word-check-search GET-ORDER
word-check-search ONLY
word-check-search ORDER
word-check-search PREVIOUS
word-check-search SEARCH-WORDLIST
word-check-search SET-CURRENT
word-check-search SET-ORDER
word-check-search WORDLIST

S"   search words total " type search-words-check @ . S"   search words missing " type search-words-missing @ . CR 
variable string-words-check 0 string-words-check ! 
variable string-words-missing 0 string-words-missing ! 
: word-check-string bl word dup find nip 0<> if count type S"   word OK" type CR 
else count type S"   word missing" type cr string-words-missing 
@ 1+ string-words-missing ! then string-words-check @ 1+ string-words-check ! ; 
word-check-string BLANK
word-check-string CMOVE
word-check-string CMOVE>
word-check-string COMPARE
word-check-string /STRING
word-check-string -TRAILING
word-check-string REPLACES
word-check-string SEARCH
word-check-string SLITERAL
word-check-string SUBSTITUTE
word-check-string UNESCAPE

S"   string words total " type string-words-check @ . S"   string words missing " type string-words-missing @ . CR 

S" core words total " type core-words-check @ . S" core words missing " type core-words-missing @ . CR 
S" coreext words total " type coreext-words-check @ . S" coreext words missing " type coreext-words-missing @ . CR 
S"   block words total " type block-words-check @ . S"   block words missing " type block-words-missing @ . CR 
S"   double words total " type double-words-check @ . S"   double words missing " type double-words-missing @ . CR 
S" exceptions words total " type exceptions-words-check @ . S" exceptions words missing " type exceptions-words-missing @ . CR  
S"   facility words total " type facility-words-check @ . S"   facility words missing " type facility-words-missing @ . CR 
S"   file words total " type file-words-check @ . S"   file words missing " type file-words-missing @ . CR 
S"   float words total " type float-words-check @ . S"   float words missing " type float-words-missing @ . CR 
S"   locals words total " type locals-words-check @ . S"   locals words missing " type locals-words-missing @ . CR 
S"   memory words total " type memory-words-check @ . S"   memory words missing " type memory-words-missing @ . CR 
S"   search words total " type search-words-check @ . S"   search words missing " type search-words-missing @ . CR 
S"   string words total " type string-words-check @ . S"   string words missing " type string-words-missing @ . CR 
S"   tools words total " type tools-words-check @ . S"   tools words missing " type tools-words-missing @ . CR 
S"   xchar words total " type xchar-words-check @ . S"   xchar words missing " type xchar-words-missing @ . CR 
