

/*****************************************************************************/
/*                                                                           */
/*  CROBOTS                                                                  */
/*                                                                           */
/*  (C) Copyright Tom Poindexter, 1985, all rights reserved.                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/* screena.c - low level screen display routines */
/*	      change or modify this module for different systems */


#include <stdio.h>
#include "crobots.h"
#ifdef _WIN32
#include <Windows.h>
#endif


#define COLS  80
#define LINES 25
void clear()
{
#ifdef _WIN32
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
#else
  printf("\033[H\033[2J");
#endif
}

void move(int y,int x)
{
#ifdef _WIN32
	COORD pos = { x, y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
#else
  printf("\033[%d;%dH",y,x);
#endif
}

/* dummy refresh function */

void refresh() {}

#ifdef LATTICE
/* use lattice's console i/o routines instead of stdio */
#define addch  putch
#define printw cprintf
#else
/* use stdio i/o routines for xenix cross compiler */
#define addch  putchar
#define printw printf
#endif

/* init_disp - initialize display */

void init_disp()
{
  clear();
  draw_field();
}


/* end_disp - cleanup and end display */

void end_disp() {}


/* playfield characters */
/* these are the line drawing characters of the PC */
#define UL_CORN 0xda
#define UR_CORN 0xbf
#define LL_CORN 0xc0
#define LR_CORN 0xd9
#define VERT	0xb3
#define HORZ	0xc4

/* exploding shell characters */
/* these are full and shaded box characters of the PC */
#define CENTER	0xdb
#define DIAG_R	0xb0
#define DIAG_L	0xb0
#define SIDE	0xb2
#define TOP_BOT 0xb2
/* the flying shell is the diamond */
#define SHELL	0x04

/* structure for explosions */
struct {
  int xx;
  int yy;
  int val;
} exp_pos [9] = {
  {  0,  0, CENTER  },
  { -1,  0, SIDE    },
  {  1,  0, SIDE    },
  {  0, -1, TOP_BOT },
  {  0,  1, TOP_BOT },
  { -1, -1, DIAG_L  },
  {  1, -1, DIAG_R  },
  { -1,  1, DIAG_R  },
  {  1,  1, DIAG_L  }
};

#define STAT_WID 20   /* width of characters for status boxes */

static int f_width;  /* play field width */
static int f_height; /* play field height */

static int col_1;    /* column for damage & speed */
static int col_2;    /* column for scan & heading */
static int col_3;    /* column for cpu cycle count*/



/* draw_field - draws the playing field and status boxes */

void draw_field()
{
  int i, j;

  /* init fixed screen data; 0,0 is top left, LINES-1,COLS-1 is lower right */
  f_width = COLS - STAT_WID - 3;  /* columns available */
  f_height= LINES - 3;		  /* lines available */


  /* top line */
  move(1,1);
  addch(UL_CORN);
  for (i = 1; i <= f_width+1; i++) {
    addch(HORZ);
  }
  addch(UR_CORN);

  /* middle lines */
  for (i = 2; i <= f_height+2; i++) {
    move(i,1);
    addch(VERT);
    move(i,COLS-STAT_WID);
    addch(VERT);
  }

  /* bottom line */
  move(LINES,1);
  addch(LL_CORN);
  for (i = 1; i <= f_width+1; i++) {
    addch(HORZ);
  }
  addch(LR_CORN);

  /* status boxes -- CAUTION: this is dependent on MAXROBOTS */
  for (i = 0; i < MAXROBOTS; i++) {
    move(5*i+1,COLS-STAT_WID+1);
    printw(" %1d %-14s",i+1,robots[i].name);
    move(5*i+2,COLS-STAT_WID+1);
    printw("  D%%       Sc    ");
    move(5*i+3,COLS-STAT_WID+1);
    printw("  Sp       Hd    ");
/*
    move(5*i+4,COLS-STAT_WID+1);
    printw("  X=       Y=    ");
*/
    if (i < MAXROBOTS-1) {
      move(5*i+5,COLS-STAT_WID+1);
      for (j = 0; j < 19; j++)
	addch(HORZ);
    }
  }
  move(LINES,COLS-STAT_WID+1);
  printw(" CPU Cycle:       ");

  /* init columns for damage, speed; scan, heading */
  col_1 = COLS - STAT_WID + 6;
  col_2 = COLS - STAT_WID + 15;
  col_3 = COLS - STAT_WID + 12;

  refresh();

}



/* plot_robot - plot the robot position */

void plot_robot(int n)
{
  int i, k;
   int new_x, new_y;

  new_x = (int) (((long)((robots[n].x+(CLICK/2)) / CLICK) * f_width) / MAX_X);
  new_y = (int) (((long)((robots[n].y+(CLICK/2)) / CLICK) * f_height) / MAX_Y);
  /* add one to x and y for playfield offset in screen, and inverse y */
  new_x += 2;;
  new_y = f_height - new_y;
  new_y +=2;

  if (robots[n].last_x != new_x || robots[n].last_y != new_y) {
    /* check for conflict */
    k = 1;
    for (i = 0; i < MAXROBOTS; i++) {
      if (i == n || robots[n].status == DEAD)
	continue; /* same robot as n or inactive */
      if (new_x == robots[i].last_x && new_y == robots[i].last_y) {
	k = 0;
	break;	  /* conflict, robot in that position */
      }
    }
    if (k) {
      if (robots[n].last_y >= 0) {
	move(robots[n].last_y,robots[n].last_x);
	addch(' ');
      }
      move(new_y,new_x);
      addch(n+'1');  /* ASCII dependent */
      refresh();
      robots[n].last_x = new_x;
      robots[n].last_y = new_y;
    }
  }
}


/* plot_miss - plot the missile position */

void plot_miss(int r,int n)
{
  int i, k;
   int new_x, new_y;

  new_x = (int) (((long)((missiles[r][n].cur_x+(CLICK/2)) / CLICK) 
		  * f_width) / MAX_X);
  new_y = (int) (((long)((missiles[r][n].cur_y+(CLICK/2)) / CLICK) 
		  * f_height) / MAX_Y);
  /* add one to x and y for playfield offset in screen, and inverse y */
  new_x += 2;
  new_y = f_height - new_y;
  new_y += 2;;

  if (missiles[r][n].last_xx != new_x || missiles[r][n].last_yy != new_y) {
    /* check for conflict */
    k = 1;
    for (i = 0; i < MAXROBOTS; i++) {
      if (robots[i].status == DEAD)
	continue; /* inactive robot */
      if ((new_x == robots[i].last_x && new_y == robots[i].last_y)  ||
	  (missiles[r][n].last_xx == robots[i].last_x &&
	   missiles[r][n].last_yy == robots[i].last_y)) {
	k = 0;
	break;	  /* conflict, robot in that position */
      }
    }
    if (k) {
      if (missiles[r][n].last_yy > 1) {
	move(missiles[r][n].last_yy,missiles[r][n].last_xx);
	addch(' ');
      }
      move(new_y,new_x);
      addch(SHELL);
      refresh();
      missiles[r][n].last_xx = new_x;
      missiles[r][n].last_yy = new_y;
    }
  }
}



/* plot_exp - plot the missile exploding */

void plot_exp(int r,int n)
{
  int c, i, p, hold_x, hold_y, k;
   int new_x, new_y;

  if (missiles[r][n].count == EXP_COUNT) {
    p = 1;  /* plot explosion */
    /* erase last missile postion */
    /* check for conflict */
    k = 1;
    for (i = 0; i < MAXROBOTS; i++) {
      if (robots[i].status == DEAD)
	continue; /* inactive robot */
      if (missiles[r][n].last_xx == robots[i].last_x &&
	  missiles[r][n].last_yy == robots[i].last_y) {
	k = 0;
	break;	  /* conflict, robot in that position */
      }
    }
    if (k) {
      if (missiles[r][n].last_yy > 1) {
	move(missiles[r][n].last_yy,missiles[r][n].last_xx);
	addch(' ');
      }
    }
  }
  else
    if (missiles[r][n].count == 1)
      p = 0; /* last count, remove explosion */
    else
      return;  /* continue to display explosion */

  hold_x = (int) (((long)((missiles[r][n].cur_x+(CLICK/2)) / CLICK) 
		   * f_width) / MAX_X);
  hold_y = (int) (((long)((missiles[r][n].cur_y+(CLICK/2)) / CLICK) 
                   * f_height) / MAX_Y);

  for (c = 0; c < 9; c++) {
    new_x = hold_x + exp_pos[c].xx;
    new_x += 2;;
    new_y = f_height - hold_y + exp_pos[c].yy;
    new_y += 2;

    /* check for off of playfield */
    if (new_x <= 1 || new_x > f_width+2 || new_y <= 1 || new_y > f_height+2)
      continue;

    k = 1;
    for (i = 0; i < MAXROBOTS; i++) {
      if (robots[i].status == DEAD)
	continue;
      if (new_x == robots[i].last_x && new_y == robots[i].last_y) {
	k = 0;
	break;	  /* conflict */
      }
    }
    if (k) {
      move(new_y,new_x);
      addch((p) ? exp_pos[c].val : ' ');
    }
  }
  refresh();
}


/* robot_stat - update status info */

void robot_stat(int n)
{
  int changed = 0;

  if (robots[n].last_damage != robots[n].damage) {
    robots[n].last_damage = robots[n].damage;
    move(5*n+2,col_1);
    printw("%03d",robots[n].last_damage);
    changed = 1;
  }
  if (robots[n].last_scan != robots[n].scan) {
    robots[n].last_scan = robots[n].scan;
    move(5*n+2,col_2);
    printw("%03d",robots[n].last_scan);
    changed = 1;
  }
  if (robots[n].last_speed != robots[n].speed) {
    robots[n].last_speed = robots[n].speed;
    move(5*n+3,col_1);
    printw("%03d",robots[n].speed);
    changed = 1;
  }
  if (robots[n].last_heading != robots[n].heading) {
    robots[n].last_heading = robots[n].heading;
    move(5*n+3,col_2);
    printw("%03d",robots[n].heading);
    changed = 1;
  }

/*
  move(5*n+4,col_1);
  printw("%3d",robots[n].x / CLICK);
  move(5*n+4,col_2);
  printw("%3d",robots[n].y / CLICK);
*/

  if (changed)
    refresh();
}


void show_cycle(long l)
{
  move(LINES,col_3);
  printw("%7ld",l);
  refresh();
}

