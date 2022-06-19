/***************************************************************************
                          fltdjglobalsearch.cpp  -  description
                             -------------------
    begin                : Sun Mar 28 2004
    copyright            : (C) 2004 by Kartik Patel
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

//Last modified 17 Apr 2004

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FL/filename.H>
#include "fltdj.h"

extern void shownote (void);	
extern void getdayname (int i);

//text typed in find box of note or contact window
extern char findtext[50];

extern char commontext[];
extern char notefilename[];
extern char grepfilename[];
extern FILE *grepfile;

extern int daybutton[];
extern int date;
//numerical representation of month
extern int month;
//numerical representation of year
extern int year;

char *get_month_name (char *monthpointer);
int get_month_integer (char *monthname);
int read_and_search_file (char *notefilename);

//this is the maximum number of lines in the global searchlist
int maxjournalsearchlines;
int maxapptsearchlines;

//these store the year and month after global search
char yearpointer[5], monthpointer[3];
char resultstr[256];
char *resultpointer;

void find_text_in_all_months (void)
{
char resultstr[256];
char *resultpointer;
int i, j;
dirent **filelist;
int number_of_files;

	//put search string in searchtext
	if (findtextbox->value())
		strcpy (findtext, findtextbox->value());
	searchtext->value (findtext);

	//clear and put search results in searchresultlist browser widget
	journalsearchresultlist->clear ();
	apptsearchresultlist->clear();

	//get list of all noting files
	sprintf (commontext, "%s/.fltdj", getenv ("HOME"));
	number_of_files = fl_filename_list (commontext, &filelist, fl_numericsort);
	//printf ("%i\n", number_of_files);
	for (i = 0; i < number_of_files; i++) {
		if (fl_filename_match (filelist[i]->d_name, "*-*.tdj")) {
			sprintf (commontext, "%s/.fltdj/%s", getenv ("HOME"), filelist[i]->d_name);
			read_and_search_file (commontext);
		}		
	}	
	
	//sprintf (commontext, "");
	 strcpy (commontext, "");
     maxjournalsearchlines = journalsearchresultlist->size ();
     maxapptsearchlines = journalsearchresultlist->size ();

     //free filelist structure
     for (i = number_of_files; i > 0;) {
		free((void*)(filelist[--i]));
	}
}

//end find_text_in_all_months

char name [11];
char *get_month_name (char *monthpointer)
{
	if (strcmp (monthpointer, "1") == 0)
		strcpy (name, "January\0");
	if (strcmp (monthpointer, "2") == 0)
		strcpy (name, "February\0");
	if (strcmp (monthpointer, "3") == 0)
		strcpy (name, "March\0");
	if (strcmp (monthpointer, "4") == 0)
		strcpy (name, "April\0");
	if (strcmp (monthpointer, "5") == 0)
		strcpy (name, "May\0");
	if (strcmp (monthpointer, "6") == 0)
		strcpy (name, "June\0");
	if (strcmp (monthpointer, "7") == 0)
		strcpy (name, "July\0");
	if (strcmp (monthpointer, "8") == 0)
		strcpy (name, "August\0");
	if (strcmp (monthpointer, "9") == 0)
		strcpy (name, "September\0");
	if (strcmp (monthpointer, "10") == 0)
		strcpy (name, "October\0");
	if (strcmp (monthpointer, "11") == 0)
		strcpy (name, "November\0");
	if (strcmp (monthpointer, "12") == 0)
		strcpy (name, "December\0");

	return name;
}

//end get_month_name

void searchlistshow_cb (Fl_Browser *obj, void *data)
{
int searchline, mnth, i;
const char *thislinedata = 0;
char yearpart[5], monthpart[4], datepart[3];

	save_data ();

     strcpy (yearpart, "");
     strcpy (monthpart, "");
     strcpy (datepart, "");
     
     //get the number of the selected line
     searchline = obj->value();
     if (searchline == 0)//nothing selected
          return;
     //get the text in this line
     thislinedata = obj->text(searchline);

     //get the year and convert to int
     strncpy (yearpart, &thislinedata[6], 4);
     strcpy (&yearpart[4], "\0");
     year = atoi (yearpart);

     //get the date and convert to int
     strncpy (datepart, &thislinedata[17], 2);
     strcpy (&datepart[2], "\0");
     date = atoi (datepart);

     //get the first 3 letters of the month and convert to int
     strncpy (monthpart, &thislinedata[27], 3);
     strcpy (&monthpart[3], "\0");
     month = get_month_integer (monthpart);
     
     get_cal ();

	for (i = 0; i < 10; i++) {
		appttime[i]->label("");
		apptname[i]->label("");

		appttime[i]->redraw();
		apptname[i]->redraw();
	}
     //construct the notefilename to open
     sprintf (notefilename, "%s/.fltdj/%s-%i.tdj", getenv ("HOME"), yearpart, month);
 	//read notes and appointments for this month
	open_journal_file ();

     //set the correct dates on the calendar buttons
	set_labels ();
 	//show the holidays for this month
     show_hol_buttons ();
 	//make the first of the month the date to be displayed
 	//date = 1;
 	//label it red, others black
  	//the date associated with calendar button day[i] is given by daybutton[i]
	for (i = 1; i <= 37; i++) {//scan the calendar button array
		day[i]->labelcolor(FL_BLACK);//set button color
       	if (date == daybutton[i]) {
       		day[i]->labelcolor(FL_RED);//set this button color
			day[i]->redraw ();
			getdayname(i);
  		}
	}

     shownote ();
	if (obj == journalsearchresultlist)
		basetab->value (journaltab);
	if (obj == apptsearchresultlist) {
		basetab->value (apptab);
		tab_cb (basetab, 0);
	}

     return;
}

//end searchlistshow_cb

int get_month_integer (char *monthname)
{
int mnth;

	//get the int corresponding to month name
	if (strcmp (monthname, "Jan\0") == 0)
		mnth = 1;
	if (strcmp (monthname, "Feb\0") == 0)
		mnth = 2;
	if (strcmp (monthname, "Mar\0") == 0)
		mnth = 3;
	if (strcmp (monthname, "Apr\0") == 0)
		mnth = 4;
	if (strcmp (monthname, "May\0") == 0)
		mnth = 5;
	if (strcmp (monthname, "Jun\0") == 0)
		mnth = 6;
	if (strcmp (monthname, "Jul\0") == 0)
		mnth = 7;
	if (strcmp (monthname, "Aug\0") == 0)
		mnth = 8;
	if (strcmp (monthname, "Sep\0") == 0)
		mnth = 9;
	if (strcmp (monthname, "Oct\0") == 0)
		mnth = 10;
	if (strcmp (monthname, "Nov\0") == 0)
		mnth = 11;
	if (strcmp (monthname, "Dec\0") == 0)
		mnth = 12;

     return mnth;
}

//end get_month_integer

int s_attach[32];
char *s_noting[32];
int s_modifiedfile;

char s_appointment[32][25][4][20];
int s_appt[32];

int read_and_search_file (char *s_notefilename)
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
char *notehit, *appthit1, *appthit2;
char *c;

FILE *s_notefile;

	//first clear everything
	for (i = 1; i <= 31; i++) {
		//length of attached note is zero
		s_attach[i] = 0;
		//nothing in the note text, fill it with nulls
		//clear the notes if they exist
		if (s_noting[i]) {
			free (s_noting[i]);//free the memory
			s_noting[i] = NULL;//NULL the pointer
		}
		//no appointments
		s_appt[i] = 0;
		for (j = 1; j <= 24; j++) {
			strcpy (s_appointment[i][j][0], "");
			strcpy (s_appointment[i][j][1], "");
			strcpy (s_appointment[i][j][2], "");
			strcpy (s_appointment[i][j][3], "");
		}
	}

	s_notefile = fopen (s_notefilename, "r");
	if (s_notefile == NULL) {//no such file exists
		return 0;
	}
	else {//read attachment line, value is length of attached note
		for (i = 1; i <= 31; i++) {
			fscanf (s_notefile, "%i", &(s_attach[i]) );
		}
		//dummy read to skip to next line
		s_noting[0] = (char *) calloc(1, 4);
		fgets (s_noting[0], 4, s_notefile);
		//and read notes and put them in the correct note
		for (i = 1; i <= 31; i++) {
			if (s_attach[i]) {
				//allocate required number of characters, plus 1 for end NULL
				s_noting[i] = (char *) calloc(1, s_attach[i] + 1);
				fread (s_noting[i], s_attach[i], 1, s_notefile);
				strcat (s_noting[i], "\0");//add a null
			}
		}

		//read 31 numbers which are the number of appointments for each day
		//some will be zero
		for (i = 1; i <= 31; i++) {
			fscanf (s_notefile, "%i", &(s_appt[i]));
		}
		//dummy read to skip to next line
		fgets (s_appointment[0][0][0], 20, s_notefile);
		//read the daily appointment data
		for (i = 1; i <= 31; i++) {
			if (s_appt[i]){//any appointments to read today?
				//read them
				for (j = 0; j <= s_appt[i]; j++) {
					fread (&(s_appointment[i][j]), 80, 1, s_notefile);
				}
			}//if
		}//for
		fclose (s_notefile);
	}//else

	//locate searchtext
	for (i = 1; i <= 31; i++) { //scan all the days
		if (i == 31)//end of the month
			notehit = appthit1 = appthit2 = 0;

		//search the notes
		if (s_attach[i]) { //note exists for this day
			//check it; this is non-zero if text is found
			notehit = strstr (s_noting[i], findtext);
         		if (notehit) { //string found
				//locate directory name
				resultpointer = strstr (s_notefilename, ".fltdj");
				//copy four characters after 7th character - this is the year
				strncpy (yearpointer, &resultpointer[7], 4);
				strcpy (&yearpointer[4], "\0");
				//copy two characters after 12th character - this is the month
				strncpy (monthpointer, &resultpointer[12], 2);
				//compare second char with separator "dot"
				if (strcmp(&monthpointer[1], ".\0") == 0) {
					strcpy (&monthpointer[1], "\0");//yes - single digit month
				}
				else//no, double digit month
					strcpy (&monthpointer[2], "\0");
				//make string to display, after converting month digit to char
				sprintf (resultstr, "Year: %s Date: %.2i Month: %s",
					yearpointer, i, get_month_name (&monthpointer[0]));
					//put it in the results list
				journalsearchresultlist->add (resultstr);
			}
		}

		//search the appts
		if (s_appt[i] > 0) {//appointments exist for this day
			for (j = 1; j <= s_appt[i]; j++) {//scan all appts for this day
				//check the two fields of appts; non-zero if text is found
				appthit1 = strstr (s_appointment[i][j][0], findtext);
                   	appthit2 = strstr (s_appointment[i][j][1], findtext);
				if (appthit1 || appthit2) {//string found
					//locate directory name
					resultpointer = strstr (s_notefilename, ".fltdj");
					//copy four characters after 7th character - this is the year
					strncpy (yearpointer, &resultpointer[7], 4);
					strcpy (&yearpointer[4], "\0");
					//copy two characters after 12th character - this is the month
					strncpy (monthpointer, &resultpointer[12], 2);
					//compare second char with separator "dot"
					if (strcmp(&monthpointer[1], ".\0") == 0) {
						strcpy (&monthpointer[1], "\0");//yes - single digit month
					}
					else//no, double digit month
						strcpy (&monthpointer[2], "\0");
					//make string to display, after converting month digit to char
					sprintf (resultstr, "Year: %s Date: %.2i Month: %s",
						yearpointer, i, get_month_name (&monthpointer[0]));
					//put it in the results list
					apptsearchresultlist->add (resultstr);
				}
			}
		}
	}
		
	return 1;
}

//end read_and_search_file

//end file


