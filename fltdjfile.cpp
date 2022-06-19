/***************************************************************************
                          fltdjfile.c
                          -----------
    File I/O

    begin                : Tue Sep 17 2000
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

//Last modified - 16 Apr, 2004

#include <FL/fl_ask.H>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "fltdj.h"

struct HOLIANN {
	char date[4];
	char month[3];
	char description[26];
};

FILE *notefile;
extern FILE *holiannfile;

extern char notefilename[128];
extern char todofilename[128];
extern char contactfilename[128];
extern char helpfilename[128];
extern char holiannfilename[128];
extern char prefsfilename[128];

extern int attach[32];
extern char *noting[32];
extern int modifiedfile;

extern char appointment[32][25][4][20];
extern int appt[32];
extern int topapptindex;
extern int apptboxindex;
extern int maxapptindex;
extern int apptindex;
extern struct HOLIANN holiann[101];
extern int maxholindex;
extern int editorfont, editorfontsize;
extern int showappointmentswithnotes, showfortune;
extern int searchallmonths, searchappointments;

int open_journal_file ()
{
/*
Journal file format:
First line: 31 integers - these represent the lengths of the note for each day of the month
Then from the next line there are 31 notes for each day. Each note starts on a separate line
and is equal in length as per the coresponding number in the first line.
After this, on a separate line by itself, there are 31 integers, each denoting the number of appointments
on each day of the month, each appointment being 80 chars long.
This is followed by the appointments data.
*/

int i, j;
char *c;

	//first clear everything
	for (i = 1; i <= 31; i++) {
		//length of attached note is zero
		attach[i] = 0;
		//nothing in the note text, fill it with nulls
		//clear the notes if they exist
		if (noting[i]) {
			free (noting[i]);//free the memory
			noting[i] = NULL;//NULL the pointer
		}		
		//no appointments
		appt[i] = 0;
		for (j = 1; j <= 24; j++) {
			strcpy (appointment[i][j][0], "");
			strcpy (appointment[i][j][1], "");
			strcpy (appointment[i][j][2], "");
			strcpy (appointment[i][j][3], "");
		}
	}

	notefile = fopen (notefilename, "r");
	if (notefile == NULL) {//no such file exists
		return 0;
	}
	else {//read attachment line, value is length of attached note
		for (i = 1; i <= 31; i++) {
			fscanf (notefile, "%i", &(attach[i]) );
		}
		//dummy read to skip to next line
		noting[0] = (char *) calloc(1, 4);
		fgets (noting[0], 4, notefile);
		//and read notes and put them in the correct note
		for (i = 1; i <= 31; i++) {
			if (attach[i]) {
				//allocate required number of characters, plus 1 for end NULL
				noting[i] = (char *) calloc(1, attach[i] + 1);
				fread (noting[i], attach[i], 1, notefile);
				strcat (noting[i], "\0");//add a null
			}
		}
	
		//read 31 numbers which are the number of appointments for each day
		//some will be zero
		for (i = 1; i <= 31; i++) {
			fscanf (notefile, "%i", &(appt[i]));
		}
		//dummy read to skip to next line
		fgets (appointment[0][0][0], 20, notefile);
		//read the daily appointment data
		for (i = 1; i <= 31; i++) {
			if (appt[i]){//any appointments to read today?
				//read them
				for (j = 0; j <= appt[i]; j++) {
					fread (&(appointment[i][j]), 80, 1, notefile);
					//printf ("%s %s %s %s\n", appointment[i][j][0], appointment[i][j][1], appointment[i][j][2], appointment[i][j][3]);
				}
			}//if
		}//for
		fclose (notefile);
	}//else
	modifiedfile = 0;
	return 1;
}

//end open_file

int save_journal_file ()
{
int i, j;
int somethingtosave = 0;

	//check if there is anything to save
	for (i = 1; i <= 31; i++)
		somethingtosave += attach[i] + appt[i];
	if (!somethingtosave) {
		unlink (notefilename);
		modifiedfile = 0;
		return 0;
	}

	notefile = fopen (notefilename, "w");

	//write 31 numbers which are the lengths of the attached notes
	//some will be zero
	for (i = 1; i <= 31; i++) {
		fprintf (notefile, "%i ", attach[i]);
	}
	//go to next line
	fprintf (notefile, "\n");
	
	//write the notes
	for (i = 1; i <= 31; i++) {
		if (attach[i]) {//this does NOT write the terminating null
			fwrite (noting[i], attach[i], 1, notefile);
		}
	}
	fprintf (notefile, "\n");
	
	//write 31 numbers which are the number of appointments for each day
	//some will be zero
	for (i = 1; i <= 31; i++) {
		fprintf (notefile, "%i ", appt[i]);
	}
	fprintf (notefile, "\n");
	
	//write the appointments
	for (i = 1; i <= 31; i++) {
		if (appt[i])//any appointments to save today?
			//write them all
			for (j = 0; j <= appt[i]; j++) {
				fwrite (&(appointment[i][j]), 80, 1, notefile);
			}
	}
	
	fclose (notefile);
	modifiedfile = 0;	
	return 1;
}

//end save_journal_file

extern char *contact[];
extern int contactlength[];
extern char contactname[];
extern int contactindex;
extern int maxcontacts;
extern int modifiedcontacts;
extern char commontext[];

int open_contact_file (void)
//return 1 if there is a problem, else return 0
{
int i;
size_t j;
FILE *contactfile;

	contactfile = fopen (contactfilename, "r");
	if (contactfile == NULL) {
		strcpy (commontext, "\0");
		//make the string for the system command
		sprintf (commontext, "touch %s/.fltdj/contact.tdj",
			getenv ("HOME"));
		//create the file
		int status = system (commontext);		
		return 0;
	}
		
	//no of contacts	
	fscanf (contactfile, "%i", &maxcontacts);
	//read length of each contact
	for (i = 1; i <= maxcontacts; i++) {
		fscanf (contactfile, "%i", &(contactlength[i]));
	}
	//allocate the memory required for each contact
	for (i = 1; i <= maxcontacts; i++) {
		contact[i] = (char *) calloc (1, contactlength[i]);
	}
	//dummy read to go to next line
	fgets (noting[0], 4, contactfile);
	//read the contact data
	for (i = 1; i <= maxcontacts; i++) {
		fread (contact[i], contactlength[i], 1, contactfile);
		//strcat (contact[i], "\0");
	}
	
	fclose (contactfile);
	contactindex = 1;
	modifiedcontacts = 0;

	//first clear the drop down list
	contactkeylist->clear ();
	//then add first line of each contacts to drop down contact list
	for (i = 1; i <= maxcontacts; i++) {
		//find place where first newline occurs
		j = strcspn(contact[i], "\n");
		j = (j > 18) ? 18 : j;
		//clear contactname
		strncpy (contactname, "", 19);
		//copy characters upto newline into contactname
		strncpy (contactname, contact[i], j);
		//add it to contact key drop down list
		contactkeylist->add (&(contactname[0]), &i);
	}
	contactkeylist->redraw();
	return 0;
}

//end open_contact_file

void save_contact_file (void)
{
int i;
FILE *contactfile;

	sort_contacts ();
	
	contactfile = fopen (contactfilename, "w");
	if (contactfile == NULL) {
		fl_alert ("Attention!\n Error in saving contact data file %s/.fltdj/contact.tdj",
			getenv ("HOME"));
		return;
	}
	
	//no of contacts
	fprintf (contactfile, "%i ", maxcontacts);
	//length of each contact data
	for (i = 1; i <= maxcontacts; i++) {
		fprintf (contactfile, "%i ", contactlength[i]);	
	}
	//skip to next line
	fprintf (contactfile, "\n");
	//write contact data, without writing the end NULL
	for (i = 1; i <= maxcontacts; i++) {
		fwrite (contact[i], contactlength[i], 1, contactfile);
	}
	
	fclose (contactfile);
	
	for (i = 1; i <= maxcontacts; i++) {
		//free (contact[i]);
	}
}

//end save_contact_file

void save_todo_file (void)
{
extern Fl_Text_Buffer *todobuf;
FILE *todofile;

	//todofile = fopen (todofilename, "w");
	todobuf->savefile (todofilename);
	//fclose (todofile);
}

//end save_todo_file

void readholiannfile (void)
{
int i;

	holiannfile = fopen (holiannfilename, "r");
	//write the number of holidays
	fscanf (holiannfile, "%i", &maxholindex);
	//dummy read to go to next line, holiann[0] is not used
	fgets (&(holiann[0].date[0]), 4, holiannfile);
	//read the holidays
	for (i = 1; i <= maxholindex; i++) {
		fread (&(holiann[i]), sizeof (struct HOLIANN), 1, holiannfile);
	}
	fclose (holiannfile);
}

//end readholiannfile

void writeholiannfile (void)
{
int i;
FILE *holiannfile;

	holiannfile = fopen (holiannfilename, "w");
	//write the number of holidays
	fprintf (holiannfile, "%i", maxholindex);
	//go to next line
	fprintf (holiannfile, "\n");
	//write the holidays
	for (i = 1; i <= maxholindex; i++) {
		fwrite (&holiann[i], sizeof (struct HOLIANN), 1, holiannfile);
	}
	fclose (holiannfile);
}

//end writeholiannfile (void)

void save_prefs (void)
{
FILE *prefsfile;

	prefsfile = fopen (prefsfilename, "w");
	fprintf (prefsfile, "%i %i %i %i %i %i",
		editorfont, editorfontsize,
  		showappointmentswithnotes, searchappointments,
		searchallmonths, showfortune
  		);
	fclose (prefsfile);
}

//end save_prefs

void read_prefs (void)
{
FILE *prefsfile;

	//default values
	editorfont = FL_TIMES;
	editorfontsize = 12;

	showappointmentswithnotes = 0;
	showfortune = 0;

	searchappointments = 0;
	searchallmonths = 0;

	prefsfile = fopen (prefsfilename, "r");
	
	if (prefsfile == NULL) {//no preferences file found, so defaults prevail
		return;
	}
	else {
		fscanf (prefsfile, "%i %i %i %i %i %i",
			&editorfont, &editorfontsize,
   			&showappointmentswithnotes, &searchappointments,
			&searchallmonths, &showfortune
      		);
		fclose (prefsfile);
	}
	//error check in case some values are missing (such as when new version
	//of fltdj reads older version prefs file)
	if (editorfont < 0 || editorfont > 15)
		editorfont = FL_TIMES;
	if (editorfontsize < 1 || editorfontsize > 64)
		editorfontsize = 12;
	if (showappointmentswithnotes < 0 || showappointmentswithnotes > 1)
		showappointmentswithnotes = 0;
	if (searchallmonths < 0 || searchallmonths > 1)
		searchallmonths = 0;
	if (searchappointments < 0 || searchappointments > 1)
		searchappointments = 0;
	if (showfortune < 0 || showfortune > 1)
		showfortune = 0;
}

//end read_prefs
