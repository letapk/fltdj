/***************************************************************************
                          fltdjnotes.cpp  -  description
                             -------------------
    begin                : Sun Aug 17 2003
    copyright            : (C) 2003 by Kartik Patel
    email                : letapk@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//Last modified - 21 Mar 2004

#include <stdlib.h>
#include <string.h>

#include "fltdj.h"

struct HOLIANN {
	char date[3];
	char month[3];
	char description[26];
};

extern void getdayname (int i);

//data variables for notes========================================================
//length of note which is attached to any day. Use starts from 1, attach[0] is
//not used
int attach[32];
//text of note which has been attached to any day
//Use starts from 1, noting[0] is reserved for future use.
char *noting[32];
//flag to indicate if note has been modified or not
int modifiedfile = 0;
//========================================================

//buffers for the various editor data
Fl_Text_Buffer *notebuf;
//=================================================

//these are declared and defined in fltdjcal.cpp
extern int offset;
extern int month;
extern int date;
extern int currentdaybuttonindex;
extern int daybutton[];

//these are declared and defined in fltdjholidays.cpp
extern int maxholindex;
extern struct HOLIANN holiann[101];

//these are declared and defined in fltdjprefs.cpp
extern int showfortune;

//these are declared and defined in fltdjmain.cpp
extern char fortunefilename[128];

void shownote (void)
{
int i, j;

	getdayname (currentdaybuttonindex);
 	notebuf->text ("\0");//clear the note buffer
	todaysholiday1->hide();//box showing holiday info
	if (!attach[date] && showfortune && basetab->value() != apptab) {
	//show fortune if there is no note and apptab is not visible (or fortune command
	//gets displayed on appt number.
		make_a_fortune ();
		notebuf->loadfile (fortunefilename);
	}
	else if (noting[date])//or load the note buffer if there is one
		notebuf->text(noting[date]);
	noteditor->redraw();

	//make string for display of holiday box
	for (j = 1; j <= maxholindex; j++) {
		if (month == atoi (holiann[j].month)) {//for this month which is displayed on calendar
			if (date == atoi (holiann[j].date)) {//found a holiday!
				todaysholiday1->show();
				todaysholiday1->label(holiann[j].description);
			}
		}
	}
}

//end shownote

//callback for some non-date buttons on the calendar and the tabs
void note_buttons (Fl_Widget *obj, void *data)
{
int i = 0;
Fl_Button *but;

	but = (Fl_Button *) obj;

	//these are on the note tab
	if (obj == prevnote) {//previous note
		i = currentdaybuttonindex - 1;
		if (currentdaybuttonindex > 1)
			while (!attach[daybutton[i]] && daybutton[i] > 0 && i > 1)
				i--;
			if (attach[daybutton[i]] && day[i]->visible())
				datebutton ((Fl_Widget *)day[i], NULL);
		return;
	}
	if (obj == nextnote) {//next note
		i = currentdaybuttonindex + 1;//current note plus one
		while (!attach[daybutton[i]] && daybutton[i] < 32) {//find next note
			i++;
			if (i > 37)//this fixes a weird segfault. See Changelog entry for 17 June 2002.
				return;
		}
		if (attach[daybutton[i]] && day[i]->visible())
			datebutton ((Fl_Widget *)day[i], NULL);
		return;
	}
}

//end note_buttons


void save_note_in_buffer (void)
{
int i = 0, j = 0;

	//see if there are any characters typed in
	j = notebuf->length();
	if (j) {//something in the editor
		//get the size
		attach[date] = j;
		//clear the note always
		modifiedfile = 1;
		if (noting[date])//if noting exists
			free (noting[date]);
		//allocate the required memory
		noting[date] = (char *)calloc (1, attach[date]+1);//allocate memory +1 for end NULL
		strcpy (noting[date], notebuf->text());//save the note
		strcat (noting[date], "\0");
	}
	else {//nothing in the editor
		if (noting[date]) {//if noting exists
			free (noting[date]);//free the memory
			noting[date] = NULL;//NULL the pointer
			attach[date] = 0;//zero out the size of note
		}
	}

	//square, round, up or down button type
	set_button_type (date+offset);
}

//end save_note_in_buffer

//end file
