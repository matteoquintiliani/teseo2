/* Teseo-2 Plug-in
 * Copyright (C) 2005  Stefano Pintore, Matteo Quintiliani (the "Authors").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE Authors BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Authors of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Authors.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>


#include "teseocallbacks.h"
#include "teseointerface.h"
#include "teseosupport.h"

#include "teseo_bezier_fit.h"
#include "teseo_lock.h"
#include "teseo_resample.h"
#include "teseo_session.h"
#include "teseo_main.h"
#include "teseo_wmean.h"

GtkWidget * teseowin;
GtkWidget * preferencesdlg;
GtkWidget * aboutdlg;
GtkWidget * sessiondlg;
GtkFileChooser * teseofilechooser;
GtkFileChooser * teseosessionfilechooser;

GimpDrawable * private_drawable ;
gint32  teseo_image ;

char TODO_str[] = "Sorry, this function is not implemented yet!";
char new_session_name[] = "New session ...";

int
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

 int ret=0;
 GtkEntry * teseo_imagefile_entry      =  (GtkEntry *)   lookup_widget(GTK_WIDGET(sessiondlg), "teseo_imagefile_entry");
 GtkEntry * teseo_imageresolution_entry=  (GtkEntry *)   lookup_widget(GTK_WIDGET(sessiondlg), "teseo_imageresolution_entry");


 char * image_file = gimp_image_get_filename(teseo_image);
 gdouble xresolution,yresolution;
 char str_res[80];

 gtk_entry_set_text (teseo_imagefile_entry, image_file);

 gimp_image_get_resolution (teseo_image,  &xresolution,  &yresolution);

 sprintf(str_res,"%d",(gint) xresolution);
 strcat(str_res," dpi");
 gtk_entry_set_text (teseo_imageresolution_entry, str_res);

 gtk_window_set_title (GTK_WINDOW (sessiondlg), new_session_name);

 gint result = gtk_dialog_run (GTK_DIALOG (sessiondlg));

  switch (result)
    {
      case GTK_RESPONSE_OK:
          ret=1;
	  new_session(image_file, NULL);
         break;
      case GTK_RESPONSE_DELETE_EVENT:
         break;
      default:
         break;
    }
  gtk_widget_hide (sessiondlg);
  gtk_window_set_title (GTK_WINDOW (sessiondlg), "");
  return ret;
}


void
on_properties1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
 gint result = gtk_dialog_run (GTK_DIALOG (sessiondlg));
  switch (result)
    {
      case GTK_RESPONSE_OK:
          if (!save_session(current_session))
             g_message("Unable to save current Session.");
         break;
      case GTK_RESPONSE_CANCEL:
      case GTK_RESPONSE_DELETE_EVENT:
         load_session(current_session);
         break;
      default:
         break;
    }
 gtk_widget_hide (sessiondlg);
}

//return 1 on success, 2 on user refuse, 0 on error
int
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

  char old_current_session[FILENAMELEN];
  char filename[FILENAMELEN];
  char * path=NULL;

  int ret=0;
  int ls=0;

  strcpy(old_current_session, current_session);

  path=teseo_get_environment_path( SESSIONPATH );

  gtk_file_chooser_set_current_folder(teseosessionfilechooser, path );
  free(path);

  gint result = gtk_dialog_run (GTK_DIALOG (teseosessionfilechooser));

  switch (result)
    {
      case GTK_RESPONSE_OK:
	 strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (teseosessionfilechooser)) );
         ret=load_session(filename);
         break;
      case GTK_RESPONSE_CANCEL:
	 ret=2;
         break;
      case GTK_RESPONSE_DELETE_EVENT:
	 ret=2;
         break;
      default:
         break;
    }

    gtk_widget_hide ((GtkWidget *) teseosessionfilechooser);

    if (ret==1) {
        result = gtk_dialog_run (GTK_DIALOG (sessiondlg));
        switch (result)
          {
            case GTK_RESPONSE_OK:
	       //ret=1;
               break;
            case GTK_RESPONSE_CANCEL:
            case GTK_RESPONSE_DELETE_EVENT:
  	       gtk_window_set_title (GTK_WINDOW (sessiondlg), "");
               ls=load_session(old_current_session);
	       ret=2;
               break;
            default:
               break;
          }
        gtk_widget_hide (sessiondlg);
    }
    if (ret==0) {
           g_message("Unable to open session.");
    }
    return ret;
}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_preferences_2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_svg1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // Import SVG
    char filename[FILENAMELEN];
    char * path=NULL;

    path=teseo_get_environment_path( SVGPATH );
    gtk_window_set_title (GTK_WINDOW (teseofilechooser), "Import bezier path from SVG file");
    gtk_file_chooser_set_current_folder(teseofilechooser, path );
    free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (teseofilechooser));

    switch (result)
    {
      case GTK_RESPONSE_OK:
	 strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (teseofilechooser)) );
	 teseo_import_svg_vectors( teseo_image, filename );
	 break;
      case GTK_RESPONSE_CANCEL:

	 break;
      case GTK_RESPONSE_DELETE_EVENT:

	 break;
      default:
	 break;
    }
    gtk_widget_hide ((GtkWidget *) teseofilechooser);

}


void
on_dxf2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

  char filename[FILENAMELEN];
  char * path=NULL;

  path=teseo_get_environment_path( DXFPATH );
  gtk_window_set_title (GTK_WINDOW (teseofilechooser), "Open DXF file");
  gtk_file_chooser_set_current_folder(teseofilechooser, path );
  free(path);

  gint result = gtk_dialog_run (GTK_DIALOG (teseofilechooser));

  switch (result)
    {
      case GTK_RESPONSE_OK:
         strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (teseofilechooser)) );
         teseo_import_dxf( teseo_image, filename );
         break;
      case GTK_RESPONSE_CANCEL:
         g_message("Cancel pressed: don't do anything.");
         break;
      case GTK_RESPONSE_DELETE_EVENT:
         g_message("Delete event, same as Cancel pressed.");
         break;
      default:
         break;
    }
  gtk_widget_hide ((GtkWidget *) teseofilechooser);
}


void
on_trace2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_sac2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_sac_xy2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_sisma2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_timemark2_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_path1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_dxf1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export DXF
    g_message(TODO_str);
}


void
on_track1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export Trace
    g_message(TODO_str);
}


void
on_sac1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export SAC
    g_message(TODO_str);
}


void
on_sac_xy1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_sisma1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export SISMA
    g_message(TODO_str);
}


void
on_timemark1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export Timemark
    g_message(TODO_str);
}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_resample1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO catch parameter for resampling_bezier
    teseo_resampling_bezier(teseo_image, 1, 1);
}


void
on_align_all1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_align_all_path(teseo_image);
}


void
on_link_all1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    teseo_link_all_path(teseo_image);
}


void
on_move_and_rotation1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_fitting_polyline1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_fitting_bezier1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    glong num_strokes;
    gdouble *strokes = NULL;
    glong num_path;
    gdouble *path_inter = NULL;
    char pathname [255] ;
    char newpathname [255] ;

    gint num_paths=0;

    gimp_path_list (teseo_image, &num_paths);

    // if at least one path exists
    if (num_paths>0) {
	
	// catch the name ot the current path
	strcpy(pathname, gimp_path_get_current(teseo_image));
	
	// convert path in strokes
	strokes = teseo_open_path_to_strokes(teseo_image, &num_strokes,  pathname);

	// fitting strokes with bezier curves
	teseo_fitting_bezier(num_strokes, strokes, &num_path, &path_inter);

	sprintf(newpathname, "%s - Fit Bezier", pathname);

	// create new path 
	gimp_path_set_points(teseo_image, newpathname, 1, num_path * 3, path_inter);

	if(path_inter) {
		free(path_inter);
	}
	if(strokes) {
		free(strokes);
	}
    }
}


void
on_recover_last2_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_evaluate_middle_tms1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_adjustment1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_info_window_1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    gint result = gtk_dialog_run (GTK_DIALOG (aboutdlg));
    switch (result)
    {
	case GTK_RESPONSE_OK:
	    break;
	case GTK_RESPONSE_DELETE_EVENT:
	    break;
	default:
	    break;
    }
    gtk_widget_hide (aboutdlg);
}




void
on_preferences_w_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

    gint result = gtk_dialog_run (GTK_DIALOG (preferencesdlg));

    switch (result)
    {
	case GTK_RESPONSE_OK:
	    g_message("OK pressed: save new preferences.");
	    g_message(TODO_str);
	    break;
	case GTK_RESPONSE_CANCEL:
	    g_message("Cancel pressed: don't save anything, restore the old preferences in the preferences dialog.");
	    g_message(TODO_str);
	    break;
	case GTK_RESPONSE_DELETE_EVENT:
	    g_message("Delete event, same as Cancel pressed.");
	    g_message(TODO_str);
	    break;
	default:
	    break;
    }
    gtk_widget_hide (preferencesdlg);

}


gboolean
on_win_teseo_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    teseo_unlock();
    gtk_main_quit();
    return TRUE;
}


gboolean
on_win_preferences_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}


void
on_dlg_preferences_response            (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{
  gtk_widget_hide(preferencesdlg);
  // return response_id;
}


void
on_dlg_preferences_close               (GtkDialog       *dialog,
                                        gpointer         user_data)
{
  gtk_widget_hide(preferencesdlg);
  // return FALSE;
}



void
on_ascii1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    g_message(TODO_str);
}


void
on_svg2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export SVG
    g_message(TODO_str);
}


void
on_ascii2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // TODO Export ASCII
    g_message(TODO_str);
}


void
on_bezier1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    // Import Bezier path (Teseo 1 format)
    char filename[FILENAMELEN];
    char * path=NULL;

    path=teseo_get_environment_path( BEZIERPATH );
    gtk_window_set_title (GTK_WINDOW (teseofilechooser), "Open Bezier Path");
    gtk_file_chooser_set_current_folder(teseofilechooser, path );
    free(path);

    gint result = gtk_dialog_run (GTK_DIALOG (teseofilechooser));

    switch (result)
    {
      case GTK_RESPONSE_OK:
	 strcpy( filename,  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (teseofilechooser)) );
	 teseo_import_bzr( teseo_image, filename );
	 break;
      case GTK_RESPONSE_CANCEL:
	 g_message("Cancel pressed: don't do anything.");
	 break;
      case GTK_RESPONSE_DELETE_EVENT:
	 g_message("Delete event, same as Cancel pressed.");
	 break;
      default:
	 break;
    }
    gtk_widget_hide ((GtkWidget *) teseofilechooser);
}

void
on_teseo_alg_back_toolbutton_clicked   (GtkButton   *button,
                                        gpointer         user_data)
{
    g_message("%s\nPress Back button.", TODO_str);
}


void
on_teseo_alg_go_toolbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
   int iter;
   GtkSpinButton * tfss = (GtkSpinButton *)   lookup_widget(GTK_WIDGET(teseowin), "teseo_forward_step_spinbutton");
   iter=gtk_spin_button_get_value_as_int (tfss) ;
   gint32 drawable_ID=  gimp_image_get_active_drawable  (teseo_image);

   teseo_main_loop(iter, drawable_ID );
}


void
on_alg_wmean_radiotoolbutton_clicked   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{

  GtkButton *teseo_alg_go_toolbutton   = (GtkButton *)   lookup_widget(GTK_WIDGET(teseowin), "teseo_alg_go_toolbutton");
  GtkButton *teseo_alg_back_toolbutton = (GtkButton *)   lookup_widget(GTK_WIDGET(teseowin), "teseo_alg_back_toolbutton");

  wmeanParams s;

  s.colour=1;
  s.step   = 1 ;
  s.width  = 5;
  s.height = 50;

  GtkSpinButton * twhs = (GtkSpinButton *)   lookup_widget(GTK_WIDGET(teseowin), "teseo_wm_height_spinbutton");
  GtkSpinButton * twws = (GtkSpinButton *)   lookup_widget(GTK_WIDGET(teseowin), "teseo_wm_width_spinbutton");
  GtkRadioButton * twcbr = (GtkRadioButton *) lookup_widget(GTK_WIDGET(teseowin), "teseo_wm_colour_black_radiobutton");

  if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(twcbr))  == TRUE ){
    s.colour=0;
    }
  else {
    s.colour=255;
    }

  s.width  = gtk_spin_button_get_value_as_int (twws);
  s.height = gtk_spin_button_get_value_as_int (twhs);

 teseo_main_init(
			(*teseo_wmean),
			(*teseo_wmean_init),
			&s,
			(*teseo_wmean_getinput),
			(*teseo_wmean_terminate),
			(*teseo_wmean_accumulate),
			(*teseo_wmean_starting_os),
			(*teseo_wmean_new_is),
			(*teseo_wmean_release)
		);

  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_go_toolbutton, TRUE);
  GTK_WIDGET_SET_FLAGS (teseo_alg_go_toolbutton, GTK_CAN_FOCUS);
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_back_toolbutton, TRUE);
  GTK_WIDGET_SET_FLAGS (teseo_alg_back_toolbutton, GTK_CAN_FOCUS);

}


/*
 Changes in the algorithms params disable go and back buttons, so the user reinitialise after changes
*/

static inline void disable_buttons(){
  GtkButton *teseo_alg_go_toolbutton   = (GtkButton *)   lookup_widget(GTK_WIDGET(teseowin), "teseo_alg_go_toolbutton");
  GtkButton *teseo_alg_back_toolbutton = (GtkButton *)   lookup_widget(GTK_WIDGET(teseowin), "teseo_alg_back_toolbutton");
  GtkRadioToolButton *ghost_radiotoolbutton = (GtkRadioToolButton *) lookup_widget(GTK_WIDGET(teseowin), "ghost_radiotoolbutton");

  //toggle the alghoritms buttons as side effect
  gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (ghost_radiotoolbutton), TRUE);
  //set go and back buttons insensitive
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_go_toolbutton, FALSE);
  GTK_WIDGET_UNSET_FLAGS (teseo_alg_go_toolbutton, GTK_CAN_FOCUS);
  gtk_widget_set_sensitive ((GtkWidget *) teseo_alg_back_toolbutton, FALSE);
  GTK_WIDGET_UNSET_FLAGS (teseo_alg_back_toolbutton, GTK_CAN_FOCUS);
}


void
on_teseo_wm_colour_black_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
 disable_buttons();
}


gint
on_teseo_wm_height_spinbutton_input    (GtkSpinButton   *spinbutton,
                                        gdouble *new_value,
                                        gpointer         user_data)
{
  //input signal once for every press on arrows
  disable_buttons();
  return TRUE;
}


gint
on_teseo_wm_width_spinbutton_input     (GtkSpinButton   *spinbutton,
                                        gdouble *new_value,
                                        gpointer         user_data)
{
  disable_buttons();
  return TRUE;
}


void
on_dlg_session_show                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
 static char session_name[FILENAMELEN];
 gchar *g_session_name = g_path_get_basename(current_session);
 strcpy(session_name, g_session_name);
 g_free(g_session_name);
 if(strcmp(gtk_window_get_title (GTK_WINDOW (sessiondlg)), new_session_name) != 0) {
  gtk_window_set_title (GTK_WINDOW (sessiondlg), session_name);
 }
}


void
on_win_teseo_show                (GtkWidget       *widget,
                                        gpointer         user_data)
{
  GtkMenuItem     *menu_open1 = (GtkMenuItem *)   lookup_widget(GTK_WIDGET(teseowin), "open1");
  GtkMenuItem     *menu_new1 = (GtkMenuItem *)    lookup_widget(GTK_WIDGET(teseowin), "new1");
  if (on_open1_activate (menu_open1, NULL) != 1)
  {
    //g_message("Open canceled");
    if (on_new1_activate (menu_new1, NULL)==0) {
      //g_message("New canceled");
      on_win_teseo_show(teseowin,NULL);
    }
  }
}

void
on_dlg_about_show_label_ver            (GtkWidget       *widget,
                                        gpointer         user_data)
{
    gchar teseo_caption_markup[200];
    sprintf(teseo_caption_markup, "<small>Open source software &#169; %s\n%s</small>", TESEO_YEAR, TESEO_CAPTION_DEV);
    gtk_label_set_label((GtkLabel *) widget, teseo_caption_markup);
}


void
on_dlg_session_show_teseo_eventpathname
                                        (GtkWidget       *widget,
                                        gpointer         user_data)
{
    GtkComboBoxEntry * combo_box = (GtkComboBoxEntry *) widget;
    gchar **path_list;
    glong n_path, i;

    // TODO insert path in teseo_eventpathname combobox and select right one....
    path_list = gimp_path_list(teseo_image, &n_path);
    for(i=0; i < n_path; i++) {
	gtk_combo_box_append_text(combo_box, path_list[i]);
    }
    

}

