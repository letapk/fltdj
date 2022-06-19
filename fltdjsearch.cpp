/***************************************************************************
                          fltdjsearch.cpp
                          ---------------
    begin                : Thu Dec 12 2002, 2003
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

//Last modified 28 Mar 2004

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <FL/Fl.H>

#include "fltdj.h"

extern void shownote (void);	
extern void getdayname (int i);

//text typed in find box of note or contact window
char findtext[50];
//index of note, appointment and contact where text was last found
int notehit = 0;
int appthit = 0;
int conhit = 0;

extern int daybutton[];
extern int attach[];
extern char *noting[];
extern int date, offset, searchappointments;
extern char commontext[];
extern char grepfilename[];
extern char notefilename[];
extern FILE *grepfile;

//these are declared and defined in fltdjappt.cpp
extern char appointment[32][25][4][20];
extern int appt[];
extern int maxapptindex;
extern int apptindex;

//these are declared and defined in fltdjcontact.cpp
extern int contactindex, maxcontacts;
extern char *contact[];

extern int currentdaybuttonindex;

//numerical representation of month
extern int month;
//numerical representation of year
extern int year;
//numerical date of the month associated with the selected daybutton
extern int date;

void find_text (void)
{
int i, j;
char *texthit1 = NULL, *texthit2 = NULL, *texthit3 = NULL, *texthit4 = NULL;
Fl_Widget *tab;

	tab = basetab->value();	
	save_data ();
		
	strcpy (findtext, "\0");
	strcpy (findtext, findtextbox->value());
	if (!strlen (findtext))//nothing to search
		return;

	if (tab == searchtab){
		unlink (grepfilename);
		find_text_in_all_months ();
	}

	if (tab == contab){//check contacts
		if (!maxcontacts)
			return;//nothing to search
		for (i = conhit + 1; i <= maxcontacts; i++) {
			if (i >= maxcontacts) {
				//i = 0;
				conhit = 0;
			}
			texthit4 = strstr (contact[i], findtext);
			if (texthit4) {
				conhit = i;
				contactindex = i;
				if (conhit >= maxcontacts)
					conhit = 0;
				display_contact (contactindex);		
				display_contactnum (contactindex);		
				return;
			}
		}
	}//if contab

	if (tab == journaltab){//check notes
		for (i = notehit + 1; i <= 31; i++) {//scan all the days
			if (i == 31)//end of the month
				notehit = 0;
			if (attach[i]) {//note exists for this day
				//check it; this is non-zero if text is found
				texthit1 = strstr (noting[i], findtext);
	         		if (texthit1) {//string found
					notehit = i;
					date = i;
			 		if (notehit >= 31)
   						notehit = 0;
				    //find the index of the button for this date
				    for (j = 1; j <= 37; j++) {//scan the calendar button array
					    if (date == daybutton[j]) {
						    currentdaybuttonindex = j;//set the button index
						    break;
					    }
				    }
				    shownote ();
				    datebutton ((Fl_Widget *)day[i+offset], NULL);
				    return;
				}//texthit1
			}//attach
		}//for i
	}//if journaltab

	if (tab == apptab){//check appts
		for (i = appthit + 1; i <= 31; i++) {//scan all the days
			if (i == 31)//end of the month
				appthit = 0;
			if (appt[i] > 0) {//appointments exist for this day
				for (j = 1; j <= appt[i]; j++) {//scan all appts for this day
					//check the two fields of appts; non-zero if text is found
					texthit2 = strstr (appointment[i][j][0], findtext);
                    	texthit3 = strstr (appointment[i][j][1], findtext);
					if (texthit2 || texthit3) {//string found
						appthit = i;
						date = i;
				 		if (appthit >= 31)
				   			appthit = 0;
						//find the index of the button for this date
						for (j = 1; j <= 37; j++) {//scan the calendar button array
							if (date == daybutton[j]) {
								currentdaybuttonindex = j;//set the button index
								break;
							}
						}
						shownote ();
          			     datebutton ((Fl_Widget *)day[i+offset], NULL);
						return;
					}//texthit
				}//for j
			}//appt[i]
		}//for i
	}//if apptab
}

//end find_text

//end file
