/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+
#+     Glade / Gtk Programming
#+
#+     Copyright (C) 2019 by Kevin C. O'Kane
#+
#+     Kevin C. O'Kane
#+     kc.okane@gmail.com
#+     https://www.cs.uni.edu/~okane
#+     http://threadsafebooks.com/
#+
#+ This program is free software; you can redistribute it and/or modify
#+ it under the terms of the GNU General Public License as published by
#+ the Free Software Foundation; either version 2 of the License, or
#+ (at your option) any later version.
#+
#+ This program is distributed in the hope that it will be useful,
#+ but WITHOUT ANY WARRANTY; without even the implied warranty of
#+ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#+ GNU General Public License for more details.
#+
#+ You should have received a copy of the GNU General Public License
#+ along with this program; if not, write to the Free Software
#+ Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#+
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

// April 14, 2019

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <glib.h>
#include <math.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

void		on_destroy();
gboolean	clock_timer_handler();
double		second = 0;
double		minute = 0;
double		hour = 0;
char		time_string[128];

void on_destroy() {
	gtk_main_quit();
}

//-----------------------------
//	main timer process
//-----------------------------
gboolean clock_timer_handler() { // check pulse every 1000 millisecs
	struct tm *tod;
	time_t tx = time(NULL);

	tod = localtime(&tx);

	strcpy(time_string, asctime(tod));
	time_string[strlen(time_string)-1] = 0;

	second = tod->tm_sec;
	minute = tod->tm_min;
	hour = tod->tm_hour % 12;
    gtk_widget_queue_draw (draw2);
    return TRUE;
}

//--------------------------
//	draw meter
//--------------------------
#define MHOR 450.0
#define MVER 350.0
#define METER_R 200.0

#define MARGIN 20
void longMashMarks(cairo_t *cr){
	double x,y,x1,y1;
	double X = 0;
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // white
	cairo_set_line_width(cr, 2.5);
	for (int i = 0; i < 60; i++ ) { // all long hash marks
		//Put marks only on 5 second intervals (0, 5, 10, ...)
		if (!(i % 5) ) { 
			//Get coordinates of outer end of long hash mark
			cairo_arc (cr, MHOR, MVER, METER_R + 10, -M_PI, -M_PI + X); //long length hash
			cairo_get_current_point (cr, &x, &y); // sets x and y
			//Get coordinates of inner end of hash mark
			cairo_arc (cr, MHOR, MVER, METER_R, -M_PI, -M_PI + X); // short length hash
			cairo_get_current_point (cr, &x1, &y1); // sets x1 and y1
			//Draw hash mark from (x,y) to (x1,y1)
			cairo_new_path (cr);
			cairo_move_to(cr, x, y);
			cairo_line_to (cr, x1, y1 );
			cairo_stroke (cr);
		}
		//		There are 30 hash marks per half circle (60 for full circle)
		//		A half circle is M_PI. A full circle is 2 * M_PI so each
		//			hash mark is M_PI / 30.0 apart.
		X = X + M_PI / 30.0; 
	}
	//draw the rectangle
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // white
	cairo_set_line_width(cr, 2.);
	cairo_move_to(cr, MHOR - METER_R - MARGIN, MVER - METER_R - MARGIN);
	cairo_line_to (cr, MHOR - METER_R - MARGIN, MVER + 1.2*METER_R + MARGIN);
	cairo_line_to (cr, MHOR + METER_R + MARGIN, MVER + 1.2*METER_R + MARGIN);
	cairo_line_to (cr, MHOR + METER_R + MARGIN, MVER - METER_R - MARGIN);
	cairo_line_to (cr, MHOR - METER_R - MARGIN, MVER - METER_R - MARGIN);
	cairo_stroke (cr);
}

void needles(cairo_t *cr, double hor, double ver, double needle_length, double needle_position, double meter_radius, int cFlg) {
	double x, x1, x2, y, y1, y2;
	ver = ver + 1.0;
	double X = 0.0;
	//	One quarter of a circle (2 * M_PI -> full circle)
	X = M_PI / 2.0;
	//------------------
	//	Pick a color
	//------------------
	if (cFlg == 1) cairo_set_source_rgb(cr, 1.0, 1.0, 0.3); // yellow
	else if (cFlg == 2) cairo_set_source_rgb(cr, 1.0, 0., 0.); // red
	else if (cFlg == 3) cairo_set_source_rgb(cr, 0.5, 1.0, 0.5); // blue

	if (cFlg == 1) {
		for (int i = 0; i< 60; i++ ) { // all short dot marks
			if (i % 5) {

				cairo_arc (cr, hor, ver, needle_length + 2, -M_PI, -M_PI + X); //long length
				cairo_get_current_point (cr, &x, &y);

				cairo_arc (cr, hor, ver, needle_length - 1, -M_PI, -M_PI + X); // short length
				cairo_get_current_point (cr, &x1, &y1);
		
				if (X > needle_position) break;
	
				cairo_new_path (cr);
				cairo_move_to(cr, x, y);

				cairo_line_to (cr, x1, y1 );

				cairo_stroke (cr);
				}
			X = X + M_PI / 30.;
			}
		}
	//----------------
	//	needle
	//----------------
	cairo_arc (cr, hor, ver, needle_length, -M_PI, -M_PI + needle_position);
	cairo_get_current_point (cr, &x, &y);
	cairo_new_path (cr);
	cairo_move_to(cr, x, y);
	cairo_line_to (cr, hor, ver );
	cairo_stroke (cr);
	//----------------
	//	spindle
	//----------------
    cairo_arc (cr, hor, ver, 4.0, -M_PI, M_PI ); // -M_PI -> 0.0 -> +M_PI : full circle
    cairo_line_to (cr, hor, ver+3 );
    cairo_fill(cr); // fill in arc
}

/*
void testcirle(cairo_t *cr){
	cairo_set_source_rgb(cr, 1.0, 0.0, 0.0); // blue
	cairo_set_line_width(cr, 2.5);
	cairo_arc(cr,106.0,76.0,50,-M_PI,+M_PI);
	cairo_stroke (cr);
}*/

gboolean on_draw2_draw (GtkDrawingArea *widget, cairo_t *cr) {
	double hor, ver, needle_length, meter_radius, needle_position;
	hor = MHOR;
	ver = MVER;
	meter_radius = METER_R;
	longMashMarks(cr);
	//----------------------------------------------------------------------
	//	note: don't alter original (hour,minute,second) variables as
	//	re-draws triggered by system will used modified values
	//----------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//	Zero for a clock it straight up. Adjust needle positions accordingly
	//	Example: if hour == zero, needle should be straight up: add 3 hours.
	//-----------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	//	Sixty minutes to an hour: Adjust hour to include how close minute
	//	hand is to the next hour.
	//---------------------------------------------------------------------------

	needle_position = hour + 3.0 + minute / 60.0;

	//--------------------------------------------------------------------------
	//	6 hours per half circle so hour marks are hour * (M_PI / 6.0) apart
	//--------------------------------------------------------------------------

	needle_position = needle_position * (M_PI / 6.0);
	needle_length = METER_R*35.0/50.0;
	needles(cr, hor, ver, needle_length, needle_position, meter_radius, 3);

	//--------------------------------
	//	minute zero adjustment
	//--------------------------------

	needle_position = minute + 15;

	//----------------------------------------------
	//	Minutes are 30 per half circle apart
	//----------------------------------------------

	needle_position = needle_position * (M_PI / 30.0);
	needle_length = METER_R*45.0/50.0;
	needles(cr, hor, ver, needle_length, needle_position, meter_radius, 2);

	//----------------------------------------------------------
	//	Seconds handles similar to minutes (60 per circle)
	//----------------------------------------------------------

	needle_position = second + 15;	// adjust zero to top of dial
	needle_position = needle_position * (M_PI / 30.0);
	needle_length = METER_R*50.0/50.0;
	needles(cr, hor, ver, needle_length, needle_position, meter_radius, 1);

	//---------------------------------
	//	Show digital text of time
	//---------------------------------

	cairo_move_to (cr, MHOR-METER_R*0.95-MARGIN+5, MVER+METER_R*1.05+MARGIN); // location: horizontal, vertical
	cairo_set_font_size(cr, 10+(int)((METER_R-50)/10));
	cairo_show_text(cr, time_string); // you can change the font if you want
	cairo_stroke (cr);


	return FALSE;
}
