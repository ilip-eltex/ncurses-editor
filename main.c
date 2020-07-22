#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <stdio.h>
#include <string.h>

#  define wcols(w)  ((w)-> _maxx+1 )
#  define wlines(w) ((w)-> _maxy+1 )

void sig_winch (int signo)
{
	struct winsize size;
	ioctl (fileno(stdout), TIOCGWINSZ, (char *) &size);
	resize_term (size.ws_row, size.ws_col);
}

int main (int argc, char ** argv)
{
	if (argc < 2) 
	{
		fprintf (stderr, "USAGE: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *f = fopen (argv[1], "rw");
	rewind (f);

	if (f == NULL)
	{
		fprintf (stderr, "Cann't open '%s'. Check file existence and available permissions\n", argv[1]);
		return 1;
	}

    	if (!initscr ())
    	{
    		fprintf (stderr, "Cann't init ncurses screen. Force stop\n");
		return 1;
    	}
	
	signal (SIGWINCH, sig_winch);	
        

    	WINDOW *win = newwin (30, 80, 5, 5);
    	wmove (win, 1, 1);
	printw ("Welcome to ncurses editor! You are editing '%s'. INSERT ONLY\nF10 Exit; F2 Save; F3 Open", argv[1]);
	refresh ();
	char c;
	
	wrefresh (win);
	keypad (win, 1);
	int winX=0, winY=0, ch;
	wmove (win, winY, winX);
	while (fread (&c, sizeof(char), 1, f) )
                wprintw (win, "%c", c);
	rewind (f);
	wmove (win, winY, winX);
	while (1)
	{
		ch = wgetch (win);
		if (ch == KEY_F(10))
			break;
		if (ch == KEY_F(2))
		{	
		
	 	int x,y;
			for (y=0; y < wlines(win); y++ ) 
				for( x=0 ; x <= wcols(win); x++ )
				{
					wmove (win, y, x);
					char tmp = winch (win) & A_CHARTEXT;
					fwrite (&tmp, sizeof(char), 1, f);
				}	
		wmove (win, 0, 0);
		}
		switch (ch)
		{
			case KEY_DOWN: 
				if ( winY+1 <= 30 ) 
				{
					wmove (win, ++winY, winX);
				}
				break;
			case KEY_UP:
				if (winY-1 >= 0) 
				{
					wmove (win, --winY, winX);
				}
				break;
			case KEY_RIGHT:
				if (winX+1 <= 80)
					wmove (win, winY, ++winX);
				break;
			case KEY_LEFT:
				if (winY-1 >= 0)
					wmove (win, winY, --winX);
				break;
					
		}
		wrefresh (win);
		
	}	
    	delwin(win);
    	endwin();


	return 0;


}
