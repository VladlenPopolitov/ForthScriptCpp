\ rook.r  -  scans the battlefield like a rook, i.e., only 0,90,180,270 */
\ move horizontally only, but looks horz and vertically */

variable course
variable boundary
variable d
0 course !
0 boundary !

: change course @ 
0= if 5 boundary ! 180 course !
   else 995 boundary ! 0 course !
   then
course @ 30 drive ;

\ * look somewhere, and fire cannon repeatedly at in-range target */
variable look_range variable look_deg 
: look look_deg ! 
 begin 
 look_deg @ 2 scan
 dup look_range ! dup 0> swap 701 - 0< and 
 while
  course @ 0 drive
  look_deg @  look_range @ cannon
  d @ 20 + damage - 
  0<>  if damage d ! change then
 repeat ;

variable main_y
: main 
 loc_y 500 < if 90 70 drive
 begin loc_y 500 - 20 < speed 0> and while noop repeat
 else
 begin loc_y 500 - 20 > speed 0> and while noop repeat
 0 0  drive
 damage d !
 0 course !
 995 boundary !
 course @ 30 drive
 begin 1 while
0 look
90 look 
180 look
270 look
course @ 0= if loc_x boundary @ > speed 0= or if change then
 else loc_x boundary @ < speed 0= or if change then
 then
repeat ;
main
 
\ end of rook.fth