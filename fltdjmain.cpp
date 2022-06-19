/***************************************************************************
                          fltdjmain.cpp
                          -------------
    The Daily Journal - A PIM program

    begin                : Sun Sep 17 2000
    copyright            : (C) Kartik Patel
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

#include <unistd.h>//for getpid and unlink
#include <stdlib.h>//for getenv
#include <string.h>//for getenv
#include <FL/fl_ask.H>
#include <FL/filename.H>
#include <FL/Fl_Help_Dialog.H>

#include "fltdj.h"

void getdayname (int i);

struct HOLIANN {
	char date[3];
	char month[3];
	char description[26];
};

//in fltdjnotes.cpp
extern void shownote (void);
extern void save_note_in_buffer (void);

void save_data (void);

Fl_Help_Dialog dummyhelpwin;

//temporary storage
char commontext[256];

//data variables for notes========================================================
//length of note which is attached to any day. Use starts from 1, attach[0] is
//not used
extern int attach[32];
//text of note which has been attached to any day
//Use starts from 1, noting[0] is reserved for future use.
extern char *noting[32];
//flag to indicate if note has been modified or not
extern int modifiedfile;
//========================================================

//data variables for file I/O=====================================================
char notefilename[128];
char todofilename[128];
char contactfilename[128];
char helpfilename[128];
char pidfilename[128];
char holiannfilename[128];
char prefsfilename[128];
char grepfilename[128];
char fortunefilename[128];
char gnulicensefilename[128];
//=================================================

//buffers for the various editor data
Fl_Text_Buffer *todobuf;
extern Fl_Text_Buffer *notebuf;//in fltdjnotes.cpp
Fl_Text_Buffer *contactbuf;
//=================================================

//data variables for the pid and pid lockfile
pid_t fltdjpid;
FILE *pidfile;
FILE *holiannfile;
FILE *fortunefile;
FILE *grepfile;
FILE *gnugplfile;
FILE *todofile;

//text variable to store GPL license
Fl_Text_Buffer gnugplbuf;
//=================================================

//these are declared and defined in fltdjcal.cpp
extern int offset;
extern int status;
extern int month;
extern int year;
extern int date;
extern int daybutton[];
extern int leap;
extern int currentdaybuttonindex;
extern char monthname[];
extern char dayname[];
extern char notedaydate[];
extern void check_date (void *);

//these are declared and defined in fltdjappt.cpp
extern char appointment[32][25][4][20];
extern int appt[];
extern int maxapptindex;
extern int apptindex;

//these are declared and defined in fltdjholidays.cpp
extern int maxholindex;
extern int holindex;
extern int holiannadded;
extern struct HOLIANN holiann[101];

//these are declared and defined in fltdjcontact.cpp
extern int contactindex;

//these are declared and defined in fltdjprefs.cpp
extern int showfortune;
extern int searchallmonths, searchappointments;
extern int editorfont, editorfontsize;

//these are declared and defined in fltdjsearch.cpp
extern int hit;

int main (int argc, char *argv[])
{
int i;

	//make the string for the fltdj directory
	sprintf (pidfilename, "%s/.fltdj", getenv ("HOME"));
	//check for fltdj directory
	i = fl_filename_isdir (pidfilename);
	if (i == 0) {//directory does not exist
		fl_message ("It seems that your home account does not have a\n\
directory named \"%s/.fltdj\". This directory is necessary,\n\
since all the data created by you will be stored here in \n\
various files. Click 'OK' to create this directory and continue.", getenv ("HOME"));
		//make the string for the system command
		sprintf (pidfilename, "mkdir %s/.fltdj", getenv ("HOME"));
		//create the directory
		status = system (pidfilename);
		fl_message ("Created \"%s/.fltdj\".\n\
Please copy the file \"help.html\" from the\n\
source package to this directory in order to\n\
access the help file from within the program.\n\
Click 'OK' to continue.", getenv ("HOME"));
	}
	
	//make the string for the pid lockfile
	sprintf (pidfilename, "%s/.fltdj/fltdjlockfile.tdj", getenv ("HOME"));
	//try to open it
	pidfile = fopen (pidfilename, "r");
	if (pidfile) {//lockfile exists
		fl_message ("It seems that \"The Daily Journal\" is already running.\n\
If this is not the case, delete the lockfile \"%s\" and restart fltdj.\n\
Click 'OK' to terminate this copy of the program", pidfilename);
		exit (0);
		
	}
	else {//no lockfile, so get pid and create it
		fltdjpid = getpid ();
		pidfile = fopen (pidfilename, "w");
		fprintf (pidfile, "%i", (int) fltdjpid);
		fclose (pidfile);
	}
	
	create_the_ui ();
	mainwin->show();
	initialize_windows ();

	set_appointment_alerts ();
	
	//enter the event loop
	Fl::run ();
}

//end main

int initialize_windows (void)
{
int i, j, k;

	month = 0;
	leap = 0;
	
	//clear all notes and appointments
	for (i = 0; i < 32; i++) {
		noting[i] = NULL;
		for (j = 0; j < 25; j++) {
			strcpy (appointment[i][j][0], "");
			strcpy (appointment[i][j][1], "");
			strcpy (appointment[i][j][2], "");
			strcpy (appointment[i][j][3], "");									
		}
	}
	//allocate something to noting[0] for dummy reads in file I/O
	noting[0] = (char *) malloc(5);
     
	//get current day and date
	get_date ();
	//get the calendar data
	get_cal ();
	//set timeout to check date every 10 minutes
	Fl::add_timeout (600.0, check_date);

	//get the environment and prefix it to the data file names
	sprintf (helpfilename, "%s/.fltdj/help.html", getenv ("HOME"));
	sprintf (gnulicensefilename, "%s/.fltdj/COPYING", getenv ("HOME"));
	sprintf (todofilename, "%s/.fltdj/todo.tdj", getenv ("HOME"));
	sprintf (contactfilename, "%s/.fltdj/contact.tdj", getenv ("HOME"));
	sprintf (notefilename, "%s/.fltdj/%i-%i.tdj", getenv ("HOME"), year, month);
	sprintf (holiannfilename, "%s/.fltdj/holiann.tdj", getenv ("HOME"));
	sprintf (prefsfilename, "%s/.fltdj/prefs.tdj", getenv ("HOME"));
	sprintf (fortunefilename, "%s/.fltdj/fortune.tdj", getenv ("HOME"));

	//initialise note buffer and editor
	notebuf = new Fl_Text_Buffer;
	noteditor->buffer (notebuf);
	//set find text to NULL
	findtextbox->value("\0");
	//read the notes and appointments		
	open_journal_file ();

	//initialise todolist buffer and editor 
	todobuf = new Fl_Text_Buffer;
	todoeditor->buffer (todobuf);
	//read todo file and load into buffer
	todofile = fopen (todofilename, "r");
	if (todofile) {
		fclose (todofile);
		todobuf->loadfile (todofilename);
	}

	//initialise contactlist buffer and editor
	contactbuf = new Fl_Text_Buffer;
	contacteditor->buffer (contactbuf);
	//read contacts data
	i = open_contact_file ();

	//initialize the help view with the help file
	help->load(helpfilename);
	dummyhelpwin.load(helpfilename);

	//read preferences and apply them
	read_prefs ();	
	apply_prefs ();

	//load the GNU GPL info in the About tab
	gnugplbuf.loadfile(gnulicensefilename);
	gnugplicense->buffer (gnugplbuf);
	
	//number the date buttons correctly	
	set_labels ();

	//read holiday data
	holiannfile = fopen (holiannfilename, "r");
	if (holiannfile) {
		fclose (holiannfile);
		readholiannfile ();//opened and closed again here
		show_hol_buttons ();
	}
	
	//color current date red when program starts
	//the date associated with calendar button day[i] is given by daybutton[i]
	for (i = 1; i <= 37; i++) {//scan the calendar button array
		if (date == daybutton[i]) {
       		day[i]->labelcolor(FL_RED);//set button color
			day[i]->redraw ();
			getdayname (i);
         		break;
		}
	}

	//display today's journal entry, if any, or show fortune, or blank window
	shownote ();
	
	for (i = 0; i < 10; i++) {
		appttime[i]->when(FL_WHEN_RELEASE || FL_WHEN_RELEASE_ALWAYS);
		apptname[i]->when(FL_WHEN_RELEASE || FL_WHEN_RELEASE_ALWAYS);
	}
	
	mainwin->redraw();
	return 0;
}

//end initialize_windows

//callback for the tabs
void tab_cb (CMultiTabs *t, void *data)
{
int i;
Fl_Widget *tab;

	save_note_in_buffer ();//else the changes in the current note, if any, disappear

	tab = t->value();
	if (tab == journaltab) {
		shownote ();
		return;
	}
	if (tab == apptab) {
		maxapptindex = appt[date];
		apptindex = 1;
		fill_appointment_boxes (maxapptindex);		
		edit_appointment (apptindex);
		return;
	}
	if (tab == todotab) {
		todoeditor->redraw ();
		return;
	}
	if (tab == contab) {
		display_contact (contactindex);	
		display_contactnum (contactindex);
		contacteditor->redraw ();
		return;
	}
	if (tab == holtab) {
		holindex = 1;
		fill_hol_boxes (maxholindex);
		display_holnum (maxholindex);
		edit_hol (holindex);
		highlight_holtext_box (holindex);			
		return;
	}
	if (tab == preftab) {
		fontslider->value(editorfont);
		fontsizeslider->value(editorfontsize);

		fonttextbox->labelfont(editorfont);
		fonttextbox->labelsize(editorfontsize);
		fonttextbox->redraw();

		fortune->value (showfortune);
	}
   
	if (tab == searchtab){
	}	

	return;
}

//end tab_cb

//callback for global buttons - find and exit
void buttons (Fl_Widget *obj, void *data)
{
int i = 0;
Fl_Button *but;

	but = (Fl_Button *) obj;
	//save_data ();
	if (obj == exitbut) {//exit program
		getdayname (currentdaybuttonindex);
		save_data ();
		clean_up_and_exit ();		
	}	

	if (obj == findbut) {
		find_text ();//find text in visible tab
		return;	
	}
}

//end buttons

void window_cb (Fl_Widget *obj, void *data)
{
Fl_Group *o;

	o = (Fl_Group *) obj;

	//confirm exit from mainwindow
	if (!fl_ask("Close and exit 'The Daily Journal'?"))
		return;
	else{//save data
		save_data ();
		clean_up_and_exit ();
	}
	mainwin->redraw();

	if (obj != mainwin)
		return;
}


//end window_cb

void save_data (void)
{
	save_note_in_buffer ();//saves current visible note to buffer
	save_todo_file ();//saves todo list
	save_journal_file ();//saves all notes and appointments to file
	save_contact_file ();
	if (holiannadded) {
		writeholiannfile ();
		show_hol_buttons ();
	}
}

//end save_data

void clean_up_and_exit (void)
{
int i;

	i = unlink (pidfilename);//remove this or else the program hiccups the next time
	i = unlink (fortunefilename);
	i = unlink (grepfilename);
	exit (0);
}

//end clean_up_and_exit

//end file
