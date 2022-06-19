/***************************************************************************
                          fltdjappt.cpp
                          -------------
    Appointment manager

    begin                : Thu Dec 28 2000
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

//Last modified - 15 October, 2003

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Clock.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "fltdj.h"
struct HOLIANN {
	char date[3];
	char month[3];
	char description[26];
};

//data variables for appointments=================================================
//appointment array for the month, day 1 to day 31,
//24 appointments per day (1 to 24)
//4 fields: time, name, phone_number, place
//20 characters for each field
//The appointment is accessed as:
//appointment[date][apptindex][0/1/2/3]
char appointment[32][25][4][20];
//number of appointments for each day; appt[1] is the number of appointments on
//the first day of the month, etc; hence starts from 1 (there is no day 0)
int appt[32];
//index of first appointment in the appointment window
int topapptindex = 1;
//index of text box highlighted in appointment window (0 to 9)
//int apptboxindex = 0;
//index of last appointment (no appointments to start with)
int maxapptindex = 0;
//index of current appointment in input box (empty to start with)
int apptindex;
//used to set the callbact for the appointment alert
int apphour, appminute, apptime;
int currenthour, currentminute, currenttime;
int durationtoapp;
//========================================================

extern char commontext[50];
extern int daybutton[38];
extern int currentdaybuttonindex;
extern int date, offset;
extern int modifiedfile;
extern char todaydate[3];
extern char todayday[4];
extern char todaymonth[4];
extern int today_date;
extern char notedaydate[];
extern char monthname[];
extern char dayname[];
extern int year;
extern int month;

//these are declared and defined in fltdjholidays.cpp
extern int maxholindex;
extern int holindex;
extern int holiannadded;
extern struct HOLIANN holiann[101];

void appt_cb (Fl_Widget *obj, void *data)
{
int i;
Fl_Button *but;

	but = (Fl_Button *) obj;

	if (but == apptaddbut) {//add new appointment
		if (maxapptindex < 24) {
			maxapptindex++;
			if (maxapptindex > 9)//to scroll up the appointments
				topapptindex = maxapptindex - 9;
			get_appointment_from_form (maxapptindex);
			sort_appointment ();
			fill_appointment_boxes (maxapptindex);
			apptindex = maxapptindex;
			display_apptnum (maxapptindex);
			highlight_text_box (maxapptindex);
			appt[date] = maxapptindex;
			modifiedfile = 1;
			//set appt alert if it is today
			if (date == today_date) {
				//remove existing timeout, if any
				Fl::remove_timeout (appalert, 0);
				//set the next appointment timer
				set_appointment_alerts ();
			}//date
		}//maxapptindex
		else {
			maxapptindex = 24;
			fl_alert ("Only 24 appointments per day in this version!");
		}//else
		set_button_type (date+offset);
		return;
	}
	if (but == apptmodifybut) {//change appointment
		get_appointment_from_form (apptindex);
		modifiedfile = 1;
		//think of a kludge to prevent mangling of the appt index here!
		if (date == today_date) {
			//remove existing timeout, if any
			Fl::remove_timeout (appalert, 0);
			//set timer for next appointment
			set_appointment_alerts ();
		}//date
		sort_appointment ();		
		fill_appointment_boxes (apptindex);
		
		return;
	}
	if (but == apptdeletebut) {//delete appointment
		if (!maxapptindex) //nothing to delete
			return;
		delete_appointment (apptindex);
		fill_appointment_boxes (apptindex);
		maxapptindex--;
		
		//this kludge is for when the last record is being deleted
		if (apptindex > maxapptindex)
			apptindex = maxapptindex;
		//this kludge is for when the record is the first one displayed
		if (apptindex < topapptindex)
			topapptindex--;
		fill_appointment_boxes (apptindex);
		
		display_apptnum (apptindex);
		appt[date] = maxapptindex;		
		modifiedfile = 1;		
		//this kludge is when there is nothing to highlight
		if (maxapptindex == 0) {
			topapptindex = 1;
			return;
		}
		highlight_text_box (apptindex);				
		if (date == today_date) {		
			//remove existing appointment timer, if any
			Fl::remove_timeout (appalert, 0);
			//reset the timer for the next appointment
			set_appointment_alerts ();
		}//date
		return;
	}
	if (but == apptprevbut) {//previous appointment
		if (apptindex)
			apptindex--;
		else
			return;
		if (apptindex < 1) {
			apptindex = 1;
		}
		if (apptindex - topapptindex < 0) {//scroll down
			topapptindex--;
			fill_appointment_boxes (topapptindex);
		}
		edit_appointment (apptindex);
		return;
	}
	if (but == apptnextbut) {//next appointment
		if (apptindex && maxapptindex)
			apptindex++;
		else
			return;
		if (apptindex > maxapptindex) {
			apptindex = maxapptindex;
		}
		if (apptindex - topapptindex > 9) {//scroll up
			topapptindex++;
			fill_appointment_boxes (topapptindex);
		}
		edit_appointment (apptindex);
		return;
	}
	if (but == prevdayapp) {//go to prev day with appointments
		i = currentdaybuttonindex - 1;
		if (currentdaybuttonindex > 1) {
			while (!appt[daybutton[i]] && daybutton[i] > 0 && i > 1)
				i--;
			if (appt[daybutton[i]] && day[i]->visible())
				datebutton ((Fl_Widget *)day[i], NULL);
		}
		return;
	}
	if (but == nextdayapp) {//go to next day with appointments
		i = currentdaybuttonindex + 1;
		while (!appt[daybutton[i]] && daybutton[i] < 32) {
			i++;
			if (i > 37)//this fixes a weird segfault.
			//See Changelog entry for 17 June 2002.
				return;
		}
		if (appt[daybutton[i]] && day[i]->visible())
			datebutton ((Fl_Widget *)day[i], NULL);
		return;	
	}
}

//end appt_cb

//puts appointment data from input boxes into appointment array
void get_appointment_from_form (int index)
{
int i, timeinput;

	i = appttimein->size() && apptnamein->size();
	if (i) {
		strcpy (appointment[date][index][0],
			appttimein->value());
		strcpy (appointment[date][index][1],
			apptnamein->value());
		}
	//check appointment time and rectify
	timeinput = atoi (appointment[date][index][0]);
	if (timeinput < 0) {
		timeinput = 0;
		sprintf (appointment[date][index][0], "0000");
	}
	if (timeinput > 2400) {
		timeinput = 2400;
		sprintf (appointment[date][index][0], "2400");
	}
	appttimein->value(appointment[date][index][0]);
}

//end get_appointment_from_form

//displays appointments in text boxes on appt window
void fill_appointment_boxes (int index)
{
int i, j;
int boxindex;

	//first clear the boxes
	for (i = 0; i < 10; i++) {
		appttime[i]->label("");
		appttime[i]->redraw();
		apptname[i]->label("");
		apptname[i]->redraw();
	}

	//then fill the boxes
	for (i = topapptindex; i <= maxapptindex; i++) {
		boxindex = i - topapptindex;
		if (boxindex > 9)
			return;
		appttime[boxindex]->label(appointment[date][i][0]);
		apptname[boxindex]->label(appointment[date][i][1]);
	}

	todaysholiday2->hide();
	//make string for display of holiday box
	for (j = 1; j <= maxholindex; j++) {
		if (month == atoi (holiann[j].month)) {//for this month which is displayed on calendar
			if (date == atoi (holiann[j].date)) {//found a holiday!
				todaysholiday2->show();
				todaysholiday2->label(holiann[j].description);
			}
		}
	}
}

//end fill_appointment_boxes

//pushes appointments one step up
void delete_appointment (int index)
{
int i;

	//this kludge takes care of the case when index=maxapptindex
	strcpy (appointment[date][index][0], "");
	strcpy (appointment[date][index][1], "");
	
	//this is for the rest of the cases
	if (index < maxapptindex)
		for (i = index; i < maxapptindex; i++) {
			strcpy (appointment[date][i][0],
				appointment[date][i+1][0]);
			strcpy (appointment[date][i][1],
				appointment[date][i+1][1]);	
	}
	
	//clear the last appointment
	strcpy (appointment[date][maxapptindex][0], "");
	strcpy (appointment[date][maxapptindex][1], "");
}

//end delete_appointment

//puts appointment in the edit boxes
void edit_appointment (int index)
{
	appttimein->value(appointment[date][index][0]);
	apptnamein->value(appointment[date][index][1]);
	
	display_apptnum (index);
	highlight_text_box (index);
	return;
}

//end show_appointment

void highlight_text_box (int index)
{
int i;

	for (i = 0; i < 10; i++) {
		appttime[i]->labelcolor(FL_BLACK);
		apptname[i]->labelcolor(FL_BLACK);
		
		appttime[i]->labelfont(FL_TIMES);
		apptname[i]->labelfont(FL_TIMES);
	}
		
	appttime[index - topapptindex]->labelfont(FL_TIMES_BOLD);
	apptname[index - topapptindex]->labelfont(FL_TIMES_BOLD);
	
	appttime[index - topapptindex]->labelcolor(FL_RED);
	apptname[index - topapptindex]->labelcolor(FL_RED);
	
	for (i = 0; i < 10; i++) {
		appttime[i]->redraw();
		apptname[i]->redraw();
	}
	
	return;
}

//end highlight_text_box

void display_apptnum (int index)
{
	strcpy (commontext, "\0");
	if (maxapptindex)
		sprintf (commontext, "%i/%i", index, maxapptindex);
	//else
		//sprintf (commontext, "");
	
	apptnum->label(commontext);
	apptnum->redraw();
	
	return;
}

int apptcompare (const void *a, const void *b);

void sort_appointment (void)
{
	qsort (&(appointment[date][1]), maxapptindex, 80, apptcompare);
}

//end sort_appointment

int apptcompare (const void *a, const void *b)
{
long i, j;
char *c, *d;

	c = (char *) a;
	d = (char *) b;

	i = strtol ((char *)&(c[0]), (char **)NULL, 10);
	j = strtol ((char *)&(d[0]), (char **)NULL, 10);
	
	if (i <  j) return -1;
	//if (i == j) return 0;
	if (i >  j) return 1;

	return 0;
}

//end compare

void set_appointment_alerts ()
{
int i;
	
	getcurrenttime ();	
	for (i = 0; i <= appt[date]; i++) {//scan the number of appointments today
		apptime = atoi (appointment[date][i][0]);
		//alert for the next appointment that is after the current time
		if (apptime > currenttime) {
			set_app_timeout (i);
		}
	}
}

//end set_appointment_alerts

void set_app_timeout (int i)
{
	//calculate the time in seconds to next appointment
	apphour = apptime / 100;
	appminute = apptime % 100;
	durationtoapp = (apphour - currenthour) * 3600 +
			(appminute - currentminute) * 60;
			
	//set timer if there are more than 10 min (600 secs) to appointment
	if (durationtoapp > 600) {
		Fl::add_timeout (durationtoapp - 600, appalert, &(appointment[date][i][1]));
	}
	//Fl::add_timeout (durationtoapp, appalert, &(appointment[date][i][1]));
}

//end set_app_timeout

void appalert (void *name)
{
int i;
char alert_message [100];

	sprintf (alert_message,
		"Your next appointment is in 10 minutes or less!");
	alertbox->value (alert_message);
	for (i = 0; i < 1000; i++)
		fprintf (stderr, "\a\a\a\a\a\a");
	
	alertwin->show ();
	
	//set alert for the next appointment
	set_appointment_alerts ();
}

//end appalert

void alert_cb (Fl_Widget *obj, void *data)
{
	alertwin->hide();
}

//end alert_cb

void getcurrenttime (void)
{
	//this is in 24-hour format
	currenthour = Oclock->hour();
	currentminute = Oclock->minute();
	currenttime = currenthour * 100 + currentminute;
}

//end getcurrenttime

//end file
