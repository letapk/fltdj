/***************************************************************************
                          fltdjcontacts.cpp
                          -----------------
    begin                : Wed Feb 21 2001
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

#define MAXCONTACTS (500)

//data variables for contacts================================
//contact array: 500 contacts, starts from 1, 0 is not used
char *contact[MAXCONTACTS];
//length of each contact data
int contactlength [MAXCONTACTS];
//maximum number of contacts available
int maxcontacts = 0;
//index of currently displayed contact, starts from zero (blank)
int contactindex = 0;
//index of contact selected from drop down list
int selectedcontact = 0;
//string to display in contact key box
char contactname[40];
//if contact info modified;
int modifiedcontacts;
//========================================================

extern char commontext[50];
extern char findtext[50];
extern Fl_Text_Buffer *contactbuf;

//stores last contact found
int contacthit;
//flag : marks visibility of drop list
int dropcontactlistvisible = 0;

void contact_cb (Fl_Widget *obj, void *data)
{
Fl_Button *but;
Fl_Browser *browser;

	but = (Fl_Button *) obj;
	browser = (Fl_Browser *) obj;

	
	if (but == contactaddbut) {//add new contact
		//return if max. no. of contacts exceeded or nothing in editor
		if (maxcontacts == MAXCONTACTS || contactbuf->length() == 0)
			return;

		maxcontacts++;
		contactindex = maxcontacts;
		if (contacteditor->changed())
			modifiedcontacts = 1;//this seems to be always true!
		//allocate memory for this contact
		contact[contactindex] = (char *) malloc (contactbuf->length());
		strcpy (contact[contactindex], contactbuf->text());//save the note
		
		display_contact (contactindex);	
		display_contactnum (contactindex);
		
		//add contactname to drop down contact list		
		contactkeylist->add (&(contactname[0]), &contactindex);
		contactkeylist->redraw();
	}
	if (but == contactmodifybut) {//modify contact
		if (!contactbuf->length())//nothing in the buffer
			return;
		//change size of memory allocated for this contact
		contact[contactindex] = (char *) realloc (contact[contactindex], contactbuf->length());
		strcpy (contact[contactindex], contactbuf->text());//save the note
		//contacteditor->SaveTo (contact[contactindex]);
		display_contact (contactindex);		
	}
	if (but == contactdeletebut) {//delete contact
		if (!maxcontacts) //nothing to delete
			return;
		delete_contact (&contactindex);
		maxcontacts--;
		display_contact (contactindex);
		display_contactnum (contactindex);
		modifiedcontacts = 1;	
	}
	if (but == contactnextbut) {//show next contact
		if (contactindex == maxcontacts)
			return;
		contactindex++;	
		display_contact (contactindex);
		display_contactnum (contactindex);
	}
	if (but == contactprevbut) {//show previous contact
		if (contactindex == 1)
			return;
		contactindex--;	
		display_contact (contactindex);		
		display_contactnum (contactindex);
	}
	if (but == contactkeylistbut) {//drop down list of contact names
		if (!dropcontactlistvisible) {
			contactkeylist->show();
			dropcontactlistvisible = 1;
		}
		else {
			contactkeylist->hide();
			dropcontactlistvisible = 0;
		}
	}
	if (browser == contactkeylist) {//selected from drop down contact list
		selectedcontact = contactkeylist->value();
		contactindex = selectedcontact;
		if (contactindex > maxcontacts || contactindex == 0)
			contactindex = maxcontacts;
		//close drop down list
		contactkeylist->hide();
		dropcontactlistvisible = 0;
		display_contact (contactindex);		
		display_contactnum (contactindex);
		contacteditor->redraw();
	}
}

//end contact_cb

void display_contactnum (int index)
{
	strcpy (commontext, "\0");
	if (maxcontacts)
		sprintf (commontext, "%i/%i", index, maxcontacts);
	else
		//sprintf (commontext, "");
		strcpy (commontext, "");
	
	contactnum->label(commontext);
	contactnum->redraw();
	
	return;
}

//end display_contactnum

//pushes contacts one step up
void delete_contact (int *index)
{
int i, j;

	if (*index == maxcontacts) {//this is when index = maxcontacts
		contactkeylist->remove(*index);
		(*index)--;
	}
	else {//this is for the rest of the cases
		for (i = *index; i < maxcontacts; i++) {
			//strlen does NOT count the terminating NULL
			j = strlen (contact[i+1]);
			//realloc contact[i] to be equal to size of contact[i+1] + one more for terminating NULL
			contact[i] = (char *) realloc(contact[i], j + 1);
			//copy contact data
			strcpy (contact[i], contact[i+1]);
		}
	}
	
	//remove this line from the drop down list of contact names (browser)
	contactkeylist->remove(*index);
	contactkeylist->redraw();
	//free the last contact
	free (contact[maxcontacts]);
}

//end delete_contact

void display_contact (int index)
//this is the routine where contactlength is assigned! So watch it.
{
size_t i;

	if (!maxcontacts) {
		contactbuf->text ("\0");
		//contacteditor->Clear ();
		contactkey->label("");
		return;
	}
	//clear contactname
	strncpy (contactname, "", 39);	
	
	//find index where newline occurs
	i = strcspn(contact[index], "\n");
	//check for limit
	i = (i > 38) ? 38 : i;
	//copy upto that limit into contactname
	strncpy (contactname, contact[index], i);
	//show it in contactkey box
	contactkey->label(contactname);
	//clear editor prior to displaying contact details
	contactbuf->text ("\0");
	//load contact data into editor
	contactbuf->text(contact[index]);
	//get length of contact data
	contactlength[index] = contactbuf->length();
	//modify entry in drop down list of contact names (browser)
	contactkeylist->text (index, contactname);
	contactkeylist->redraw();
}

//end display_contact

void sort_contacts (void)
{
int i, j;
int swapnum;//the number of swaps that have taken place

	do {
		swapnum = 0;
		for (i = 1; i < maxcontacts; i++) {
			j = strcmp (contact[i], contact[i+1]);
			if (j > 0) {//swap contact[i] and contact[i+1]
				swap_contacts (i);
				swapnum++;
				modifiedcontacts = 1;
			}
		}
	} while (swapnum > 0);
}

//end sort_contacts

void swap_contacts (int i)
{
char *tempi, *tempj;
int j, leni, lenj;
	
	j = i + 1;
	
	//allocate memory for tempi equal to first contact
	leni = strlen (contact[i]);
	tempi = (char *) malloc (leni+1);
	
	//allocate memory for tempj equal to second contact
	lenj = strlen (contact[j]);
	tempj = (char *) malloc (lenj+1);

	//put first contact into tempi
	strcpy (tempi, contact[i]);
	strcat (tempi, "\0");
	
	//put second contact into tempj
	strcpy (tempj, contact[j]);
	strcat (tempj, "\0");

	//free the two contacts
	free (contact[i]);
	free (contact[j]);
	
	//allocate memory for first contact equal to second contact
	contact[i] = (char *) malloc (lenj+1);
	//allocate memory for second contact equal to first contact
	contact[j] = (char *) malloc (leni+1);
		
	//put second contact into first contact
	strcpy (contact[i], tempj);
	strcat (contact[i], "\0");
	
	//put first contact into second contact
	strcpy (contact[j], tempi);
	strcat (contact[j], "\0");

	//you forgot to swap the contact length data!
	contactlength[j] = leni+1;
	contactlength[i] = lenj+1;
	
	free (tempi);
	free (tempj);
}

//end swap_contacts

//end file

