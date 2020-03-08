#ifndef ForthRobotsSamples 
#define ForthRobotsSamples
#include <string>

std::string commandRook{ R"( 
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
)" },
commandTarget{ R"(: main begin again ; main )" };
std::string commandRookFast{ R"( 
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
course @ 20 drive ;

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
)" };

std::string commandRabit{ R"(
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

)" };
std::string commandSniper{ R"(\ sniper 
\ strategy: since a scan of the entire battlefield can be done in 90 
\ degrees from a corner, sniper can scan the field quickly. 

\ plot_course
variable x variable y variable curx variable cury  variable xx variable yy
: plot_course yy ! xx ! loc_x curx ! loc_y cury ! 
curx @ xx @ - x ! 
cury @ yy @ - y !
x @ 0 = 
if yy @ cury @ > 
 if 90  else 270  then 
else 
 100000 y @ * x @ / r_atan
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


\  external variables, that can be used by any function */
\ int corner;           /* current corner 0, 1, 2, or 2 */
\ int c1x, c1y;         /* corner 1 x and y */
\ int c2x, c2y;         /*   "    2 "  "  " */
\ int c3x, c3y;         /*   "    3 "  "  " */
\ int c4x, c4y;         /*   "    4 "  "  " */
\ int s1, s2, s3, s4;   /* starting scan position for corner 1 - 4 */
\ int sc;               /* current scan start */
\ int d;                /* last damage check */

variable corner
variable  c1x variable  c1y
variable  c2x variable  c2y
variable  c3x variable  c3y
variable  c4x variable  c4y
variable s1
variable s2
variable s3
variable s4
variable sc
variable d


\ new corner function to move to a different corner 

variable x variable y variable angle variable new
: new_corner
  4 r_rand new !   \ pick a random corner 
  new @ corner @ =
  if new @ 1 + 4 /MOD drop corner !
  else new @ corner ! then
 \ set new x,y and scan start 
  corner @ 0 = if c1x @ x ! c1y @ y ! s1 @ sc ! then
  corner @ 1 = if c2x @ x ! c2y @ y ! s2 @ sc ! then
  corner @ 2 = if c3x @ x ! c3y @ y ! s3 @ sc ! then
  corner @ 3 = if c4x @ x ! c4y @ y ! s4 @ sc ! then

  \ find the heading we need to get to the desired corner 
  x @ y @ plot_course dup angle !

  \ start drive train, full speed 
  100 drive

  \ keep traveling until we are within 100 meters 
  \ speed is checked in case we run into wall, other robot 
  \ not terribly great, since were are doing nothing while moving 
  begin loc_x loc_y x @ y @ distance 100 > speed 0 > and while repeat

  \ cut speed, and creep the rest of the way
  angle @ 20 drive 
  begin loc_x loc_y x @ y @ distance 10 > speed 0 > and while repeat

  \ stop drive, should coast in the rest of the way 
  angle 0 drive
; \  end of new_corner 

  variable closest        \ check for targets in range */
  variable range          \ range to target */
  variable dir            \ scan direction */

\ main 
: main 

  \ initialize the corner info */
  \ x and y location of a corner, and starting scan degree */
  10 c1x !  10 c1y !  0 s1 !
  10 c2x !  990 c2y ! 270 s2 !
  990 c3x ! 990 c3y ! 180 s3 !
  990 c4x ! 10 c4y !  90 s4 !
  9999 closest !
  new_corner       \ start at a random corner */
  damage d !       \ get current damage */
  sc @ dir !       \ starting scan direction */
  begin         \ loop is executed forever */

    begin dir @ sc 90 + < while  \ scan through 90 degree range */
      dir @ 1 scan range !    \ look at a direction */
       700 range @ > range @ 0 > and  if 
        begin range @ 0 > while     \ keep firing while in range */
          range @ closest !     \ set closest flag */
          dir @ range @ cannon  \ fire! */
          dir @ 1 scan range ! \ check target again */
          d @ 15 + damage > if  \ sustained several hits, */
            0 range !            \ goto new corner */
          then
        repeat
        dir @ 10 - dir !   \ back up scan, in case */
      then
      dir @ 2 + dir !  \ increment scan */
      d @ damage <> if \ check for damage incurred */
        new_corner          \ we're hit, move now */
        damage d !
        sc @ dir !
      then
    repeat

    9999 closest = if  \ check for any targets in range */
      new_corner             \ nothing, move to new corner */
      damage d !
      sc @ dir !
    else                      \ targets in range, resume */
      sc @ dir !
    then
    9999 closest !
  again
 ; main \  end of main */
)" };

#endif
