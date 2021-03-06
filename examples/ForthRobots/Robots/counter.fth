\ counter 
\ scan in a counter-clockwise direction (increasing degrees) 
\ moves when hit 

\ run moves around the center of the field */
variable x variable y variable i
variable last_dir
: run loc_x x ! loc_y y !
last_dir @ 0 
= if
 y @ 512 >  if
   1 last_dir ! 100 270 drive 
   begin y @ 100 - loc_y < i @ dup 1 + i ! 100 < and while repeat
   0 270 drive
 else
   1 last_dir ! 100  90 drive 
   begin y @ 100 + loc_y > i @ dup 1 + i ! 100 < and while repeat
   0  90 drive
else
 y @ 512 >  if
   0 last_dir ! 100 180 drive 
   begin x @ 100 - loc_x < i @ dup 1 + i ! 100 < and while repeat
   0 270 drive
 else
   0 last_dir ! 100   0 drive 
   begin x @ 100 + loc_x > i @ dup 1 + i ! 100 < and while repeat
   0  90 drive
then 
;

variable angle variable range variable res variable d variable ii
: main
1 res ! damage d ! 360 r_rand angle !
begin
 begin  res @ angle @ scan dup range ! 0 >  while
  range 700 > if
   50 angle @ drive
   1 ii !
   begin ii @ dup 1 + ii ! 50 < while repeat
   0 angle @ drive
   d @ damage <> if
    damage d ! run
   then
   angle @ 3 - angle !
  else
   range @ angle @ cannon
   begin range @ angle @ cannon 0 = while repeat
   d @ damage <> if
    damage d ! run
   then 
   angle @ 15 - angle !
  then
   d @ damage <> if
    damage d ! run
   then 
   angle @ res @ + angle !
   angle 360 /MOD drop angle !
 repeat
again
;                 
main
\ end of counter.fth
