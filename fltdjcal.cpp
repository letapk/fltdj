/***************************************************************************
                          fltdjcal.cpp  -  description
                             -------------------
    begin                : Tue Mar 8 2003
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

//Last modified - 27 July, 2003

#include <unistd.h>//for unlink
#include <stdlib.h>//for getenv
#include <string.h>//for getenv
#include "fltdj.h"

void getdayname (int i);

struct HOLIANN {
	char date[3];
	char month[3];
	char description[26];
};

//data variables for the calendar================================================
//text used on labels of calendar date buttons
//38 buttons of 3 chars each; useage starts from 1, not 0
char daytext[38][3];
//text representation of the year
char yearname[6];
//text representation of name of month, which is what its name is
char monthname[11];
//text representation of name of weekday, which is what its name is
char dayname[11];
//text representation of day, date and year, used in label on note window
char notedaydate[35];

//text to hold today's day, date, and month from the output of date command
char today[15];
char todaydate[3];
char todayday[4];
char todaymonth[4];

//numerical representation of today's date
int today_date;

//this is the number of unused daybuttons in the first week
int offset;
//needed for month.tdj
int status;
//numerical representation of month
int month;
//numerical representation of year
int year;
//numerical date of the month associated with the selected daybutton
int date;
//this is the array of buttons used in the calendar; the date of the month
//associated with any button of index i is given by daybutton[i]
//use of this array starts from 1, not 0
int daybutton[38];
//flag to indicate leap year
int leap;
//this is the index of currently selected day button, 0 to 38
int currentdaybuttonindex;
//=================================================

//data variables for file I/O====================================================
extern char notefilename[128];
extern char fortunefilename[128];
//=================================================

extern Fl_Text_Buffer *notebuf;//in fltdjnotes.cpp

//these are declared and defined in fltdjappt.cpp
extern int appt[];
extern int maxapptindex;
extern int apptindex;

//these are declared and defined in fltdjholidays.cpp
extern int maxholindex;
extern struct HOLIANN holiann[101];

//these are declared and defined in fltdjprefs.cpp
extern int showfortune;

//these are declared and defined in fltdjmain.cpp
extern char commontext[];
extern int attach[];
extern char *noting[];
extern int modifiedfile;
extern FILE *holiannfile;
extern FILE *fortunefile;
extern void shownote (void);	

//callback for year and month up-down buttons
//notes data file is changed here
void cal (Fl_Widget *obj, void *data)
{
int i;
Fl_Button *but;

	save_data ();
	//clear the notes buffer
 	//notebuf->text ("\0");
	
	but = (Fl_Button *) obj;
	
	if (but == monthupbutton) {
		month++;
	}
	if (but == monthdownbutton) {
		month--;
	}

	//check if month crosses year boundary (Jan or Dec), if so adjust year
	if (month == 13) {
		month = 1;
		year++;
	}
	if (month == 0) {
		month = 12;
		year--;
	}
		
	if (but == yearupbutton) {
		year++;
	}
	if (but == yeardownbutton) {
		year--;
	}
	
	//check if year is out of range
	if (year == 10000) {
		year--;
	}
	if (year == 0) {
		year++;
	}
	
	get_cal ();
	
	for (i = 0; i < 10; i++) {
		appttime[i]->label("");
		apptname[i]->label("");
		
		appttime[i]->redraw();
		apptname[i]->redraw();
	}

 	//make the filename to be read	
	sprintf (notefilename, "%s/.fltdj/%i-%i.tdj", getenv ("HOME"), year, month);	
 	//read notes and appointments for this month
	open_journal_file ();	
 	//set the correct dates on the calendar buttons
	set_labels ();
 	//show the holidays for this month
     show_hol_buttons ();	
 	//make the first of the month the date to be displayed
 	date = 1;
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

	shownote();
	if (basetab->value() == apptab) {
		maxapptindex = appt[date];
		apptindex = 1;
		fill_appointment_boxes (maxapptindex);		
		edit_appointment (apptindex);
	}
}

//end cal

//callback for calendar date buttons, to show note or appointments
void datebutton (Fl_Widget *obj, void *data)
{
int i, j, rem;
Fl_Button *but;

	save_note_in_buffer ();

 	//show the journaltab if some other tab (except apptab) is visible
	if (basetab->value() != journaltab && basetab->value() != apptab)
		basetab->value(journaltab);

	//color all the dates black
 	for (i = 1; i < 38; i++) {
		day[i]->labelcolor(FL_BLACK);
		day[i]->redraw();
	}
	//color the present one red
	but = (Fl_Button *) obj;
	but->labelcolor(FL_RED);
	//get the date and day name
	for (i = 1; i < 38; i++) {
		if (but == (day[i])) {
			getdayname (i);
			break;
		}
	}
	
	if (basetab->value() == apptab) {
		maxapptindex = appt[date];
		apptindex = 1;
		fill_appointment_boxes (maxapptindex);		
		edit_appointment (apptindex);
	}
     //this is to load the correct note in the buffer when the appt tab is visible
     shownote ();
}

//end datebutton

//assign the correct labels to all the calendar buttons
int set_labels (void)
{
int i;
	
	currentdate->label (today);

	//assign the dates for the first 7 buttons
	for (i = 1; i < 8; i++) {
		daybutton[8 - i] = daybutton[8] - i;
	}
	//assign the dates from 9th button onwards
	for (i = 1; i < 30; i++) {
		daybutton[8 + i] = daybutton[8] + i;
	}

	for (i = 1; i < 38; i++) {
		//remove the date boxes for illegal dates
		if ((daybutton[i] <= 0) || (daybutton[i] > 31)) {
			(day[i])->hide();
			continue;
		}
		if (month == 2 && leap == 0 && daybutton[i] > 28) {
			(day[i])->hide();
			continue;	//February, non-leap-year
		}
		if (month == 2 && leap == 1 && daybutton[i] > 29) {
			(day[i])->hide();
			continue;	//February, leap-year
		}
		if ((month == 4 || month == 6 || month == 9 || month == 11) \
			&& daybutton[i] > 30) {
			(day[i])->hide();
			continue;	//Apr, Jun, Sep, Nov - 30 days
		}
	
		//set label and boxtype for legal dates
		sprintf (daytext[i], "%i", daybutton[i]);
		(day[i])->label(daytext[i]);
		
		//square, round, up or down button type
		set_button_type (i);
		
		(day[i])->redraw();
		(day[i])->show();
	}

	//set year and month label
	monthtext->label(monthname);
	sprintf (yearname, "%i", year);
	monthtext->redraw();
	yeartext->label(yearname);
	yeartext->redraw();
	offset = 1 - daybutton[1];
	return 0;
}

//end set_labels

void set_button_type (int i)
{
	//normally the day buttons are up and square
	(day[i])->box(FL_UP_BOX);
	//unless the day has an attached note - round up button
	if (attach[daybutton[i]])
		(day[i])->box(FL_ROUND_UP_BOX);
	//unless the day has an appointment - down square button
	if (appt[daybutton[i]])
		(day[i])->box(FL_DOWN_BOX);
	//unless the day has both - round down button
	if (attach[daybutton[i]] && appt[daybutton[i]])
		(day[i])->box(FL_ROUND_DOWN_BOX);
	(day[i])->redraw();
}

//end set_button_type

void show_hol_buttons (void)
{
//Remember:
//the date associated with calendar button day[i] is given by daybutton[i]
int i, j;

	for (i = 1; i <= 37; i++) {//scan the calendar button array
		day[i]->color(FL_GRAY);//reset button color
		day[i]->tooltip("");
		day[i]->redraw ();
	}

	for (j = 1; j <= maxholindex; j++) {
		if (month == atoi (holiann[j].month)) {//for this month which is displayed on calendar
			for (i = 1; i <= 37; i++) {//scan the calendar button array
				if (daybutton[i] == atoi (holiann[j].date)) {//found a holiday!
					day[i]->color(FL_YELLOW);//color it yellow
					day[i]->tooltip(holiann[j].description);
					day[i]->redraw ();//redraw it
				}
			}
		}
	}
}

//end show_hol_buttons

//reads the calendar (from the 'cal' utility), gets the month and year,
//asigns the dates to the daybutton array, and labels to the calendar day buttons
//Don't confuse the (int) daybutton array with the fltk widget array of buttons
//called 'day'
int get_cal (void)
{
FILE *cal;
int i, j;

	//first clear all date-labels and reset daybuttons to 0
	for (i = 1; i < 38; i++) {
		daybutton[i] = 0;
		(day[i])->type ();
		(day[i])->label ("");
		//(day[i])->labelcolor (FL_BLACK);
 	}
	strncpy (monthname, "\0", 11);

	//invoke the external 'cal' utility and create the monthly calendar
	if (month == 0) {//program has just started, so current month
		status = system ("gcal > month.tdj");
		date = atoi (todaydate);//date at program start
		today_date = date;
	}
	else {//the month which has been chosen
		strcpy (commontext, "\0");
		sprintf (commontext, "gcal %i %i > month.tdj", month, year);
		status = system (commontext);
	}

	//open the calendar output file
	cal = fopen ("month.tdj", "r");
	//read the month and the year
	fscanf (cal, "%s %i\n", monthname, &year);
	//read and discard the dayname line
	fgets (commontext, 25, cal);
	//read and discard the dates of the first week
	fgets (commontext, 25, cal);
	//find the date of first Sunday of second row in calendar
	fscanf (cal, "%i\n", &(daybutton[8]));
	fclose (cal);

	leap = 0;
	//set leap year - if century
	if (((year % 100) == 0) && ((year % 400) == 0))
		leap = 1;
	//if plain leap year
	else if ((year % 4) == 0)
		leap = 1;

	//set the month
	if (strcmp (monthname, "January") == 0)
		month = 1;
	if (strcmp (monthname, "February") == 0)
		month = 2;
	if (strcmp (monthname, "March") == 0)
		month = 3;
	if (strcmp (monthname, "April") == 0)
		month = 4;
	if (strcmp (monthname, "May") == 0)
		month = 5;
	if (strcmp (monthname, "June") == 0)
		month = 6;
	if (strcmp (monthname, "July") == 0)
		month = 7;	
	if (strcmp (monthname, "August") == 0)
		month = 8;
	if (strcmp (monthname, "September") == 0)
		month = 9;
	if (strcmp (monthname, "October") == 0)
		month = 10;
	if (strcmp (monthname, "November") == 0)
		month = 11;
	if (strcmp (monthname, "December") == 0)
		month = 12;

	//delete the calendar output file
	i = unlink ("month.tdj");	

	return 0;
}

//end get_cal

void get_date (void)
{
FILE *cal;
int i, j;

	//invoke the date utility, create the file containing today's date
	status = system ("date > today.tdj");
	//open it
	cal = fopen ("today.tdj", "r");
	//read first 11 chars of the single line in the file; close it
	fgets (today, 11, cal);
	strcat (today, "\0");
	fclose (cal);
	
	//first 3 chars contain the day
	for (i = 0; i < 3; i++) {
		todayday[i] = today[i];
	}
	strcat (todayday, "\0");
	//leave a blank, then next 3 chars for the month
	for (i = 4, j = 0; i < 7; i++, j++) {
		todaymonth[j] = today[i];
	}
	strcat (todaymonth, "\0");
	//leave a blank, then next 2 chars the date
	for (i = 8, j = 0; i < 10; i++, j++) {
		todaydate[j] = today[i];
	}
	strcat (todaydate, "\0");
	//ignore the rest (time, timezone, and year); delete the file
	i = unlink ("today.tdj");
}

//end get_date

void check_date (void *j)
{
	get_date ();
	currentdate->label (today);
	Fl::repeat_timeout(600.0, check_date);
}

//end check_date

int make_a_fortune (void)
{
	strcpy (commontext, "\0");
	//make the string for the system command
	sprintf (commontext, "fortune > %s", fortunefilename);

	//issue system command and create the fortune file
	status = system (commontext);
	if (status < 0)
		return 0;//how unfortunate!
	else
		return 1;
}

//end make_a_fortune

void getdayname (int i)
{
int rem;

	date = daybutton[i];
	currentdaybuttonindex = i;

	rem = i % 7;
	//compare the remainder with the numerical day-of-week to get the dayname
	if (rem == 1)
		strcpy (dayname, "Sunday");
	if (rem == 2)
		strcpy (dayname, "Monday");
	if (rem == 3)
		strcpy (dayname, "Tuesday");
	if (rem == 4)
		strcpy (dayname, "Wednesday");
	if (rem == 5)
		strcpy (dayname, "Thursday");
	if (rem == 6)
		strcpy (dayname, "Friday");
	if (rem == 0)
		strcpy (dayname, "Saturday");

 	//display the weekday, date and month on the Journal and Appt tabs
	strcpy (notedaydate, "");
	sprintf (notedaydate, "%s %i %s %i", dayname, date, monthname, year);
  	notedaydatedisplayed1->label (notedaydate);
  	notedaydatedisplayed2->label (notedaydate);

	return;
}

//end getdayname

//end file
