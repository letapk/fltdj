/***************************************************************************
                          fltdjholidays.cpp
                          -----------------
    Holiday manager

    begin                : Mon Apr 28 2002
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

//Last modified - 27 July, 2003

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "fltdj.h"

//data variables for holidays=================================================
//array of holiday for the year - 100 holidays! - 1 to 100, 0 is not used
//3 fields: date (2 chars), month (2 chars), description (35 chars)

struct HOLIANN {
	char date[3];
	char month[3];
	char description[26];
} holiann[101];

int holindex, maxholindex = 0, topholindex = 1;
//flag to check if holiday data is modified
int holiannadded = 0;
extern char commontext[50];
extern int daybutton[38];
extern int currentdaybuttonindex;
extern int offset, date, month, year, leap;
extern char todaydate[3];
extern char todayday[4];
extern char todaymonth[4];
extern int today_date;
extern int leap;

void hol_cb (Fl_Widget *obj, void *data)
{
Fl_Button *but;

	but = (Fl_Button *) obj;

	if (but == holaddbut) {//add new holiday
		if (maxholindex < 100) {
			maxholindex++;
			if (maxholindex > 9)//to scroll up the holidays
				topholindex = maxholindex - 9;
			get_hol_from_form (maxholindex);
			sort_hol ();
			fill_hol_boxes (maxholindex);
			holindex = maxholindex;
			display_holnum (maxholindex);
			highlight_holtext_box (maxholindex);
			holiannadded = 1;
		}//maxholindex
		else {
			maxholindex = 100;
			fl_alert ("Sorry! Only 100 holidays and anniversaries this year. ");
		}//else
		
		return;
	}
	if (but == holdeletebut) {//delete holiday
		if (!maxholindex) //nothing to delete
			return;
		delete_hol (holindex);
		fill_hol_boxes (holindex);
		maxholindex--;
		
		//this kludge is for when the last record is being deleted
		if (holindex > maxholindex)
			holindex = maxholindex;
		//this kludge is for when the record is the first one displayed
		if (holindex < topholindex)
			topholindex--;
		fill_hol_boxes (holindex);
		
		display_holnum (holindex);
		holiannadded = 1;		
		//this kludge is when there is nothing to highlight
		if (maxholindex == 0) {
			topholindex = 1;
			return;
		}
		highlight_holtext_box (holindex);				
		return;
	}
	if (but == holprevbut) {//previous holiday
		if (holindex)
			holindex--;
		else
			return;
		if (holindex < 1) {
			holindex = 1;
		}
		if (holindex - topholindex < 0) {//scroll down
			topholindex--;
			fill_hol_boxes (topholindex);
		}
		edit_hol (holindex);
		return;
	}
	if (but == holnextbut) {//next holiday
		if (holindex && maxholindex)
			holindex++;
		else
			return;
		if (holindex > maxholindex) {
			holindex = maxholindex;
		}
		if (holindex - topholindex > 9) {//scroll up
			topholindex++;
			fill_hol_boxes (topholindex);
		}
		edit_hol (holindex);
		return;
	}
}

//end hol_cb

//puts hol data from input boxes into hol array
void get_hol_from_form (int index)
{
int monthi, datei, i;

	i = holtextin->size();
	if (i) {
		monthi = atoi (holmonthin->value());
		if (monthi < 1)
			holmonthin->value("1\0");
		if (monthi > 12)
			holmonthin->value("12\0");
		strcpy (holiann[index].month, holmonthin->value());
		holmonthin->redraw();
		
		datei = atoi (holdatein->value());
		if (datei < 1)
			holdatein->value("1\0");
		if (datei  > 31) {
			if (monthi == 2) {
				if (leap)
					holdatein->value("29\0");
				else
					holdatein->value("28\0");;
			}
			else if ((monthi == 4) || (monthi == 6) || (monthi == 9) || (monthi == 11))
				holdatein->value("30\0");
			else
				holdatein->value("31\0");
		}
		strcpy (holiann[index].date, holdatein->value());		
		holdatein->redraw();
		
		strcpy (holiann[index].description, holtextin->value());
	}
}

//end get_hol_from_form

//displays hols in text boxes on hol window
void fill_hol_boxes (int index)
{
int i;
int boxindex;

	for (i = topholindex; i <= maxholindex; i++) {
		boxindex = i - topholindex;
		if (boxindex > 9)
			return;
		holdate[boxindex]->label(holiann[i].date);
		holmonth[boxindex]->label("");
		if (atoi (holiann[i].month) == 1)
			holmonth[boxindex]->label("January");
		if (atoi (holiann[i].month) == 2)
			holmonth[boxindex]->label("February");
		if (atoi (holiann[i].month) == 3)
			holmonth[boxindex]->label("March");
		if (atoi (holiann[i].month) == 4)
			holmonth[boxindex]->label("April");
		if (atoi (holiann[i].month) == 5)
			holmonth[boxindex]->label("May");
		if (atoi (holiann[i].month) == 6)
			holmonth[boxindex]->label("June");
		if (atoi (holiann[i].month) == 7)
			holmonth[boxindex]->label("July");
		if (atoi (holiann[i].month) == 8)
			holmonth[boxindex]->label("August");
		if (atoi (holiann[i].month) == 9)
			holmonth[boxindex]->label("September");
		if (atoi (holiann[i].month) == 10)
			holmonth[boxindex]->label("October");
		if (atoi (holiann[i].month) == 11)
			holmonth[boxindex]->label("November");
		if (atoi (holiann[i].month) == 12)
			holmonth[boxindex]->label("December");
		
		holtext[boxindex]->label(holiann[i].description);
	}
}

//end fill_hol_boxes

//pushes hols one step up
void delete_hol (int index)
{
int i;

	//this kludge takes care of the case when index=maxholindex
	strcpy (holiann[index].date, "\0");
	strcpy (holiann[index].month, "\0");
	strcpy (holiann[index].description, "\0");
	
	//this is for the rest of the cases
	if (index < maxholindex)
		for (i = index; i < maxholindex; i++) {
			strcpy (holiann[i].date,
				holiann[i+1].date);
			strcpy (holiann[i].month,
				holiann[i+1].month);	
			strcpy (holiann[i].description,
				holiann[i+1].description);	
	}
	
	//clear the last hol
	strcpy (holiann[maxholindex].date, "\0");
	strcpy (holiann[maxholindex].month, "\0");
	strcpy (holiann[maxholindex].description, "\0");
}

//end delete_hol

//puts hol in the edit boxes
void edit_hol (int index)
{
double val;

	holdatein->value (holiann[index].date);
	holmonthin->value(holiann[index].month);
	holtextin->value(holiann[index].description);
	
	display_holnum (index);
	highlight_holtext_box (index);
	return;
}

//end show_hol

void highlight_holtext_box (int index)
{
int i;

	for (i = 0; i < 10; i++) {
		holdate[i]->labelcolor(FL_BLACK);
		holmonth[i]->labelcolor(FL_BLACK);
		holtext[i]->labelcolor(FL_BLACK);
		
		holdate[i]->labelfont(FL_TIMES);
		holmonth[i]->labelfont(FL_TIMES);
		holtext[i]->labelfont(FL_TIMES);
	}
		
	holdate[index - topholindex]->labelfont(FL_TIMES_BOLD);
	holmonth[index - topholindex]->labelfont(FL_TIMES_BOLD);
	holtext[index - topholindex]->labelfont(FL_TIMES_BOLD);
	
	holdate[index - topholindex]->labelcolor(FL_RED);
	holmonth[index - topholindex]->labelcolor(FL_RED);
	holtext[index - topholindex]->labelcolor(FL_RED);
	
	for (i = 0; i < 10; i++) {
		holdate[i]->redraw();
		holmonth[i]->redraw();
		holtext[i]->redraw();
	}
	
	return;
}

//end highlight_holtext_box

void display_holnum (int index)
{
	strcpy (commontext, "\0");
	if (maxholindex)
		sprintf (commontext, "%i/%i", index, maxholindex);
	else
		//sprintf (commontext, "");
		strcpy (commontext, "");
	
	holnum->label(commontext);
	holnum->redraw();
	
	return;
}

//end display_holnum

int holcompare (const void *a, const void *b);

void sort_hol (void)
{
	qsort (&(holiann[1]), maxholindex, sizeof (struct HOLIANN), holcompare);
}

//end sort_hol

int holcompare (const void *a, const void *b)
{
long idate, imonth;
long jdate, jmonth;
struct HOLIANN *c, *d;

	c = (struct HOLIANN *) a;
	d = (struct HOLIANN *) b;

	//find out the month in which the holiday falls
	imonth = strtol ((char *)(c->month), (char **)NULL, 10);
	jmonth = strtol ((char *)(d->month), (char **)NULL, 10);
	if (imonth <  jmonth)
		return -1;
	if (imonth >  jmonth)
		return 1;
	
	//if the months are the same, check the date
	if (imonth == jmonth) {	
		idate = strtol ((char *)(c->date), (char **)NULL, 10);
		jdate = strtol ((char *)(d->date), (char **)NULL, 10);
		
		if (idate <  jdate)
			return -1;
		if (idate == jdate)
			return 0;
		if (idate > jdate)
			return 1;
	}
}

//end compare

//end file
