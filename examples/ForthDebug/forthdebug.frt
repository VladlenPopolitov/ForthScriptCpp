\ words to show debug information in Forth

\ display ( addr u -- ) show dump of memory u bytes starting from addr
HEX
 : display-line ( addr -- ) hex dup 8 U.R BL EMIT dup dup 10 + swap DO I C@ 2 U.R I 1 + 4 MOD 0= IF BL EMIT THEN  LOOP BL DUP EMIT EMIT '*' EMIT BL EMIT 
  dup 10 + SWAP DO I C@ DUP 20 - 0< IF DROP '?' THEN EMIT LOOP BL EMIT '*' EMIT  
 CR ;  

: display ( addr u -- ) BASE @ ROT ROT dup 400 - 0> IF DROP 400 THEN OVER + SWAP CR DO I display-line 10 +LOOP BASE ! ;

 [UNDEFINED] FIND-CALLER [IF] : FIND-CALLER DROP S" ________" TYPE ; [THEN]
 [UNDEFINED] FIND-XT-NAME [IF] : FIND-XT-NAME DROP S" ________" TYPE ; [THEN]
: display-disasm-line ( addr -- ) hex dup 8 U.R BL EMIT DUP @ 8 U.R BL EMIT DUP FIND-CALLER BL EMIT FIND-XT-NAME CR ;

: disasm ( addr u -- )  CR BASE @ ROT ROT dup 100 - 0> IF DROP 100 THEN OVER + SWAP CR DO I display-disasm-line 4 +LOOP BASE !   ;
