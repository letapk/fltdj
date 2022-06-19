/***************************************************************************
                          fltdjfunctions.h  -  description
                             -------------------
    begin                : Sat Sep 23 2000
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

//Last modified - 7 March 2003

int initialize_windows (void);

//callbacks
void cal (Fl_Widget *obj, void *data);
void buttons (Fl_Widget *obj, void *data);
void datebutton (Fl_Widget *obj, void *data);
void appt_cb (Fl_Widget *obj, void *data);
void contact_cb (Fl_Widget *obj, void *data);
void alert_cb (Fl_Widget *obj, void *data);
void hol_cb (Fl_Widget *obj, void *data);
void window_cb (Fl_Widget *obj, void *data);
void pref_cb (Fl_Widget *obj, void *data);
void note_buttons (Fl_Widget *obj, void *data);

//file i/o
int open_journal_file (void);
int save_journal_file (void);
int open_contact_file (void);
void save_contact_file (void);
void save_todo_file (void);
void readholiannfile (void);
void writeholiannfile ();
void save_prefs (void);
void read_prefs (void);

//calendar management functions
int get_cal (void);
void get_date (void);
int set_labels (void);

//appointment management functions
void get_appointment_from_form (int index);
void fill_appointment_boxes (int index);
void delete_appointment (int index);
void edit_appointment (int index);

void highlight_text_box (int index);
void display_apptnum (int index);
void sort_appointment (void);

//void appalert (void *name);
void appalert (void *name);
void set_appointment_alerts (void);
void set_app_timeout (int i);

//holiday management functions
void get_hol_from_form (int index);
void fill_hol_boxes (int index);
void delete_hol (int index);
void edit_hol (int index);

void show_hol_buttons (void);
void highlight_holtext_box (int index);
void display_holnum (int index);
void sort_hol (void);

//contact management functions
void display_contactnum (int index);
void delete_contact (int *index);
void display_contact (int index);
void sort_contacts (void);
void swap_contacts (int i);

//other functions
void save_data (void);
void save_note_in_buffer (void);
void find_text (void);
void find_text_in_all_months (void);
void find_text_in_all_months_two (void);
void set_button_type (int i);
void clean_up_and_exit (void);
void getcurrenttime (void);
int handle (int e);
void apply_prefs (void);
int make_a_fortune (void);
