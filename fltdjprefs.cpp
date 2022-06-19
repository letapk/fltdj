/***************************************************************************
                          fltdjprefs.cpp
                          --------------
    begin                : Mon Nov 4 2002
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

//Last modified - 4 Jan 2003

#include <FL/Fl.H>
#include <FL/Fl_Slider.H>
#include "fltdj.h"

//font to be used in the editors
int editorfont, editorfontsize;
//flag; 1 if set, otherwise 0
int showappointmentswithnotes = 0;
int searchallmonths = 0;
int searchappointments = 0;
int showfortune = 0;

Fl_Check_Button *searchall;

void pref_cb (Fl_Widget *obj, void *data)
{
Fl_Button *o;

	o = (Fl_Button *)obj;
	
	editorfont = (int) fontslider->value();
	editorfontsize = (int) fontsizeslider->value();
	
	fonttextbox->labelfont(editorfont);
	
	fonttextbox->labelsize(editorfontsize);
	fonttextbox->redraw();

		
 	if (o == fortune) {
		showfortune = fortune->value();
     }

	if (o == applybut) {
		apply_prefs ();
	}
	
	if (o == savebut) {
		apply_prefs ();
		save_prefs ();
	}
	
}

//end pref_cb

void apply_prefs (void)
{
int i;

	noteditor->textfont (editorfont);
	todoeditor->textfont (editorfont);
	contacteditor->textfont (editorfont);
	journalsearchresultlist->textfont (editorfont);
	apptsearchresultlist->textfont (editorfont);
	
	noteditor->textsize (editorfontsize);
	todoeditor->textsize (editorfontsize);
	contacteditor->textsize (editorfontsize);
	journalsearchresultlist->textsize (editorfontsize);
	apptsearchresultlist->textsize (editorfontsize);
	
	noteditor->redraw();
	todoeditor->redraw();
	contacteditor->redraw();
	journalsearchresultlist->redraw();
	apptsearchresultlist->redraw();
	
	for (i = 0; i < 10; i++) {
		appttime[i]->labelfont(editorfont);
		apptname[i]->labelfont(editorfont);
	
		appttime[i]->labelsize(editorfontsize);
		apptname[i]->labelsize(editorfontsize);
		
		appttime[i]->redraw();
		apptname[i]->redraw();
		
		appttimein->textfont(editorfont);
		apptnamein->textfont(editorfont);
	
		appttimein->textsize(editorfontsize);
		apptnamein->textsize(editorfontsize);
		
		appttimein->redraw();
		apptnamein->redraw();
	}

	for (i = 0; i < 10; i++) {
		holdate[i]->labelfont(editorfont);
		holmonth[i]->labelfont(editorfont);
		holtext[i]->labelfont(editorfont);
		
		holdate[i]->labelsize (editorfontsize);
		holmonth[i]->labelsize (editorfontsize);
		holtext[i]->labelsize (editorfontsize);
	
		holdate[i]->redraw();
		holmonth[i]->redraw();
		holtext[i]->redraw();

		holdatein->textfont(editorfont);
		holmonthin->textfont(editorfont);
		holtextin->textfont(editorfont);
		
		holdatein->textsize (editorfontsize);
		holmonthin->textsize (editorfontsize);
		holtextin->textsize (editorfontsize);
	
		holdatein->redraw();
		holmonthin->redraw();
		holtextin->redraw();
	}
}

//end apply_prefs

//end file
