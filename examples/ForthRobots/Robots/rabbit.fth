\ rabbit 
\ rabbit runs around the field, randomly 
\ and never fires;  use as a target 

\ plot_course
variable x variable y variable scale variable curx variable cury  variable xx variable yy
: plot_course yy ! xx ! loc_x curx ! loc_y cury ! 
curx @ xx @ - x ! 
cury @ yy @ - y !
x @ 0 = 
if yy @ cury @ > 
 if 90  else 270  then 
else 
 100000  y @ * x @ / r_atan
 yy @ cury @ < 
 if 
  xx @ curx @ >
  if  360 
  else  180 
  then
 else 
  xx @ curx @ >
  if 0
  else  180 
  then
 then
 +
then
 ;
\ distance ( x1 y1 x2 y2 | dist )
: distance rot - dup * rot rot - dup * + r_sqrt ;

variable course
: go 2dup plot_course dup course ! 25 drive 
 begin 2dup loc_x loc_y distance 50 > while repeat
 drop drop
 course @ 0 drive
 begin speed 0 > while repeat ;


: main begin 1000 r_rand 1000 r_rand go \ go somewhere in the field  
again ; main

\ end of rabbit.r 
