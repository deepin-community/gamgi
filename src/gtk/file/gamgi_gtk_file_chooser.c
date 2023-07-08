/***********************************************
 *
 * $GAMGI/src/gtk/file/gamgi_gtk_file_import.c
 *
 * Copyright (C) 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_io_token.h"
#include "gamgi_io_file.h"

/**********************************************************
 * 1) file,folder must be smaller than GAMGI_IO_SHORTNAME *
 *                                                        *
 * 2) path must be smaller than GAMGI_IO_PATHNAME         *
 *                                                        *
 * 3) fullname (including protocol, hostname, pathname    *
 * and filename) must be smaller than GAMGI_IO_FULLNAME   *
 **********************************************************/

static gamgi_bool static_timer (void *data);
static void static_select_folders (GtkWidget *widget, void *data);

static char home[GAMGI_IO_PATHNAME] = "";
static char current[GAMGI_IO_PATHNAME] = "";
static char last[GAMGI_IO_PATHNAME] = "";

static int static_dir (char *fullname)
{
struct stat pathname;

if (stat (fullname, &pathname) < 0) return 0;

return (S_ISDIR (pathname.st_mode));
}

static gamgi_bool static_timer (void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *treeview;

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "tree_folders");
g_signal_handlers_unblock_by_func (treeview, static_select_folders, window);

return FALSE;
}

static gamgi_bool static_hide_folders (char *folder, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_folders_hide");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_check (name) == TRUE)
  return gamgi_io_file_patterns (folder, name);

return FALSE;
}

static gamgi_bool static_show_folders (char *folder, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_folders_show");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_check (name) == TRUE)
  return gamgi_io_file_patterns (folder, name);

return TRUE;
}

static gamgi_bool static_hide_files (char *file, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_files_hide");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_check (name) == TRUE)
  return gamgi_io_file_patterns (file, name);

return FALSE;
}

static gamgi_bool static_show_files (char *file, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_files_show");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_check (name) == TRUE)
  return gamgi_io_file_patterns (file, name);

return TRUE;
}

static void static_list_folders (const char *path, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *treeview;
GtkListStore *store;
GtkTreeIter iter;
DIR *directory;
struct dirent *child;
char pathname[GAMGI_IO_PATHNAME];
int length;

/*****************************************************
 * path < GAMGI_IO_PATHNAME, otherwise ignore excess *
 *****************************************************/

gamgi_io_token_cut (path, pathname, GAMGI_IO_PATHNAME);
length = strlen (pathname);

/****************************************
 * block signal connected to treeview   *
 * changes to avoid reentrant callbacks *
 ****************************************/

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "tree_folders");
g_signal_handlers_block_by_func (treeview, static_select_folders, window);

/*********************
 * build store model *
 *********************/

store = gtk_list_store_new (1, G_TYPE_STRING);

directory = opendir (path);
if (directory != NULL)
  {
  if (path != strrchr (path, '/'))
    {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, "..", -1);
    }

  while ((child = readdir (directory)) != NULL)
    {
    if (strcmp (child->d_name, ".") == 0 || strcmp (child->d_name, "..") == 0)
      continue;

    /*********************************************************************
     * path + child->d_name < GAMGI_IO_PATHNAME, otherwise ignore excess *
     *********************************************************************/

    gamgi_io_token_cut (child->d_name, pathname + length, GAMGI_IO_PATHNAME - length);
    if (static_dir (pathname) == TRUE)
      {
      if (static_hide_folders (child->d_name, window) == TRUE) continue;
      if (static_show_folders (child->d_name, window) == FALSE) continue;

      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, child->d_name, -1);
      }
    }

  /*****************************
   * save last valid directory *
   *****************************/

  strcpy (last, path);
  }

closedir (directory);

/*********************************************************
 * set model (and automatically remove the previous one) *
 *********************************************************/

gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);

/***************
 * sort column *
 ***************/

gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (store), 
0, GTK_SORT_ASCENDING);

/********************************************************
 * unblock signal inside a timer, otherwise the signal  *
 * is unblocked before the treeview set up is completed *
 ********************************************************/

g_timeout_add (0, static_timer, window);
}

static void static_list_files (const char *path, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *treeview;
GtkListStore *store;
GtkTreeIter iter;
DIR *directory;
struct dirent *child;
char fullname[GAMGI_IO_FULLNAME];
int length;

/*****************************************************
 * path < GAMGI_IO_PATHNAME, otherwise ignore excess *
 *****************************************************/

gamgi_io_token_cut (path, fullname, GAMGI_IO_PATHNAME);
length = strlen (fullname);

/*********************
 * build store model *
 *********************/

store = gtk_list_store_new (1, G_TYPE_STRING);

directory = opendir (path);
if (directory != NULL)
  {
  while ((child = readdir (directory)) != NULL)
    {
    if (strcmp (child->d_name, ".") == 0 || strcmp (child->d_name, "..") == 0)
      continue;

    /***********************************************************************
     * child->d_name < GAMGI_IO_SHORTNAME, otherwise ignore excess         *
     *                                                                     *
     * GAMGI_IO_FULLNAME must be >= GAMGI_IO_PATHNAME + GAMGI_IO_SHORTNAME *
     ***********************************************************************/

    gamgi_io_token_cut (child->d_name, fullname + length, GAMGI_IO_SHORTNAME);
    if (static_dir (fullname) == FALSE)
      {
      if (static_hide_files (child->d_name, window) == TRUE) continue;
      if (static_show_files (child->d_name, window) == FALSE) continue;

      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, child->d_name, -1);
      }
    }
  }

closedir (directory);

/*********************************************************
 * set model (and automatically remove the previous one) *
 *********************************************************/

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "tree_files");
gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);

/***************
 * sort column *
 ***************/

gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (store),
0, GTK_SORT_ASCENDING);
}

static void static_update (char *path, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *label;
GtkWidget *entry;

/************************
 * set path, clean file *
 ************************/

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_path");
gtk_label_set_text (GTK_LABEL (label), path);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_file");
gtk_entry_set_text (GTK_ENTRY (entry), "");

static_list_folders (path, window);
static_list_files (path, window);
}

static void static_select_folders (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
GtkWidget *button;
GtkWidget *label;
char pathname[GAMGI_IO_PATHNAME];
const char *path;
char *name;
char *start;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_path");
path = gtk_label_get_text (GTK_LABEL (label));

selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
  gtk_tree_model_get (model, &iter, 0, &name, -1);

  if (strcmp (name, "..") == 0)
    {
    strcpy (pathname, path);
    start = strrchr (pathname, '/');
    *start = '\0';
    start = strrchr (pathname, '/');
    *(start + 1) = '\0';
    }
  else
    {
    /*******************************************************
     * check that path + child->d_name < GAMGI_IO_PATHNAME *
     *******************************************************/

    sprintf (pathname, "%s%s/", path, name);
    }

  free (name);
  static_update (pathname, window);
  }

/*****************************************************
 * click last button as selected dir is the last dir *
 *****************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_last");
gtk_button_clicked (GTK_BUTTON (button));
}

static void static_select_files (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
GtkWidget *entry;
char *name;

selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_file");
  gtk_tree_model_get (model, &iter, 0, &name, -1);
  gtk_entry_set_text (GTK_ENTRY (entry), name);
  free (name);
  }

}

static void static_places (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
char pathname[GAMGI_IO_PATHNAME];

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_root");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  strcpy (pathname, "/");
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_home");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  strcpy (pathname, home);
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_current");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  strcpy (pathname, current);
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_last");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  strcpy (pathname, last);
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_http");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  strcpy (pathname, "http://");
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_ftp");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  strcpy (pathname, "ftp://");
  }

static_update (pathname, window);
}

/******************** internal function ****************
 *                                                     *
 *                      STATIC_SWITCH                  *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, int tag, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label;
const char *path;

/***********************
 * go back to last dir *
 ***********************/

if (tag == 0)
  {
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_path");
  path = gtk_label_get_text (GTK_LABEL (label));

  static_list_folders (path, window);
  static_list_files (path, window);
  }
}

gamgi_bool gamgi_gtk_file_chooser_ok (gamgi_window *window, char *fullname)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *label;
const char *name;
int length;

/*********************************
 * the path selected by the user *
 *********************************/

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_path");
name = gtk_label_get_text (GTK_LABEL (label));
if (gamgi_io_token_alpha_scan (name, fullname,
GAMGI_IO_FONT_ASCII, GAMGI_IO_PATHNAME) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Filename", window);
  return FALSE;
  }

length = strlen (name);

/*********************************
 * the file selected by the user *
 *********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_file");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, fullname + length,
GAMGI_IO_FONT_ASCII, GAMGI_IO_FULLNAME - length) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Filename", window);
  return FALSE;
  }

return TRUE;
}

void gamgi_gtk_file_chooser_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *button, *button_root, *button_home, *button_current, *button_last;
GtkWidget *entry;
GtkRequisition size;
char *name;

/****************************************************
 * set size, in case label_path is too large to fit *
 ****************************************************/

gtk_widget_size_request (GTK_WIDGET (dialog), &size);
gtk_widget_set_size_request (dialog, size.width, size.height);

/***********************
 * start switch signal *
 ***********************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
g_signal_connect (notebook, "switch_page",
G_CALLBACK (static_switch), window);

/*******************
 * default filters *
 *******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_folders_hide");
gtk_entry_set_text (GTK_ENTRY (entry), ".*");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_files_hide");
gtk_entry_set_text (GTK_ENTRY (entry), ".*");

/*******************************
 * check and initialize Places *
 *******************************/

button_root = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_root");
button_home = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_home");
button_current = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_current");
button_last = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_last");

name = getenv ("HOME");
if (name == NULL || gamgi_io_token_size_check (name,
GAMGI_IO_PATHNAME - 1) == FALSE) gtk_widget_set_sensitive (button_home, FALSE);
else { strcpy (home, name); strcat (home, "/"); }

if (getcwd (current, sizeof (current)) == NULL ||
gamgi_io_token_size_check (current, GAMGI_IO_PATHNAME - 1) == FALSE)
  gtk_widget_set_sensitive (button_current, FALSE);
else strcat (current, "/");

if (strcmp (last, "") != 0) button = button_last;
else if (gtk_widget_is_sensitive (button_current) == TRUE) button = button_current;
else if (gtk_widget_is_sensitive (button_home) == TRUE) button = button_home;
else button = button_root;

gtk_button_clicked (GTK_BUTTON (button));
}

void gamgi_gtk_file_chooser_location (GtkWidget *notebook, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *label;
GtkWidget *button;
GtkWidget *entry;
GtkWidget *frame;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_page, *vbox_center, *vbox_top;
GtkWidget *scrolled_window;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;

/*****************
 * Location page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Location");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**********
 * Places *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Places");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Root");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_places), window);
g_object_set_data (G_OBJECT (dialog), "button_root", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Home");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_places), window);
g_object_set_data (G_OBJECT (dialog), "button_home", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Current");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_places), window);
g_object_set_data (G_OBJECT (dialog), "button_current", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Last");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_places), window);
g_object_set_data (G_OBJECT (dialog), "button_last", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "HTTP");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_places), window);
g_object_set_data (G_OBJECT (dialog), "button_http", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "FTP");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_places), window);
g_object_set_data (G_OBJECT (dialog), "button_ftp", button);
gtk_widget_show (button);

/******************
 * Folders, Files *
 ******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (hbox_left), frame, TRUE, TRUE, 0);
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_container_add (GTK_CONTAINER (frame), scrolled_window);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
gtk_widget_show (scrolled_window);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), TRUE);
gtk_widget_set_size_request (treeview, 300, 250);
g_signal_connect (treeview, "cursor-changed",
G_CALLBACK (static_select_folders), window);
g_object_set_data (G_OBJECT (dialog), "tree_folders", treeview);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Folders", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (hbox_left), frame, TRUE, TRUE, 0);
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_container_add (GTK_CONTAINER (frame), scrolled_window);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
gtk_widget_show (scrolled_window);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), TRUE);
gtk_widget_set_size_request (treeview, 300, 250);
g_signal_connect (treeview, "cursor-changed",
G_CALLBACK (static_select_files), window);
g_object_set_data (G_OBJECT (dialog), "tree_files", treeview);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Files", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

/**************
 * Path, File *
 **************/

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Path");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

label = gtk_label_new ("");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_path", label);
gtk_widget_show (label);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("File");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, TRUE, TRUE, 0);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_IO_PATHNAME - 1);
g_object_set_data (G_OBJECT (dialog), "entry_file", entry);
gtk_widget_show (entry);
}

void gamgi_gtk_file_chooser_filters (GtkWidget *notebook, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_page, *vbox_center, *vbox_top, *vbox_top_top;

/****************
 * Filters page *
 ****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Filters ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 30);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/******************
 * Folders, Files *
 ******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Folders");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new (" Hide");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_FILE_PATTERNS);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_FILE_PATTERNS);
g_object_set_data (G_OBJECT (dialog), "entry_folders_hide", entry);
gtk_widget_show (entry);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);
 
label = gtk_label_new ("Show");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_FILE_PATTERNS);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_FILE_PATTERNS);
g_object_set_data (G_OBJECT (dialog), "entry_folders_show", entry);
gtk_widget_show (entry);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Files");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new (" Hide");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_FILE_PATTERNS);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_FILE_PATTERNS);
g_object_set_data (G_OBJECT (dialog), "entry_files_hide", entry);
gtk_widget_show (entry);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Show");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_FILE_PATTERNS);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_FILE_PATTERNS);
g_object_set_data (G_OBJECT (dialog), "entry_files_show", entry);
gtk_widget_show (entry);
}
