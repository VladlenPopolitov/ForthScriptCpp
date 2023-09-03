\ Forth fp tests - run all tests

\ Adjust the file paths as appropriate to your system
\ Select the appropriate test harness, either the simple tester.fr
\ or the more complex ttester.fs 

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