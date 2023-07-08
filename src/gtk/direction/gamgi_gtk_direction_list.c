/*******************************************************
 *
 * $GAMGI/src/gtk/direction/gamgi_gtk_direction_list.c
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"

#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_direction_create.h"
#include "gamgi_gtk_direction_list.h"
#include "gamgi_phys_direction.h"
#include "gamgi_io_token.h"

static struct {

char *indices;
int range[6]; } list;

static gamgi_bool static_range_increase (int *new, int *range)
{
int *old;
gamgi_bool change;

/******************************************************
 * each range is started as -INT_MAX, INT_MAX, to     *
 * ensure that it will always be replaced by new data *
 ******************************************************/

old = list.range;

/*****************
 * get new range *
 *****************/

range[0] = (old[0] < new[0]) ? old[0] : new[0];
range[1] = (old[1] > new[1]) ? old[1] : new[1];
range[2] = (old[2] < new[2]) ? old[2] : new[2];
range[3] = (old[3] > new[3]) ? old[3] : new[3];
range[4] = (old[4] < new[4]) ? old[4] : new[4];
range[5] = (old[5] > new[5]) ? old[5] : new[5];

/**************
 * get change *
 **************/

change = FALSE;
if (range[0] < old[0]) change = TRUE;
if (range[1] > old[1]) change = TRUE;
if (range[2] < old[2]) change = TRUE;
if (range[3] > old[3]) change = TRUE;
if (range[4] < old[4]) change = TRUE;
if (range[5] > old[5]) change = TRUE;
 
return change;
}

static gamgi_bool static_range_decrease (int value, int *range_new)
{
char *indices;
int *range;
gamgi_bool change;
int ii, jj, kk;
int i, j, k;
int b, c;

if (value == TRUE) return FALSE;

/*************************************************
 * Exclude instruction: try to reduce array size *
 *************************************************/

range = list.range;
indices = list.indices;

/******************************************************
 * each range is started as -INT_MAX, INT_MAX, to     *
 * ensure that it will always be replaced by new data *
 ******************************************************/

range_new[0] = INT_MAX; range_new[1] = -INT_MAX;
range_new[2] = INT_MAX; range_new[3] = -INT_MAX;
range_new[4] = INT_MAX; range_new[5] = -INT_MAX;

/******************
 * get real range *
 ******************/

b = range[3] - range[2] + 1;
c = range[5] - range[4] + 1;

for (i = range[0]; i <= range[1]; i++)
  {
  ii = i - range[0];

  for (j = range[2]; j <= range[3]; j++)
    {
    jj = j - range[2];

    for (k = range[4]; k <= range[5]; k++)
      {
      kk = k - range[4];

      if (indices[b * c * ii + c * jj + kk] == TRUE)
        {
        if (i < range_new[0]) range_new[0] = i;
        if (i > range_new[1]) range_new[1] = i;
        if (j < range_new[2]) range_new[2] = j;
        if (j > range_new[3]) range_new[3] = j;
        if (k < range_new[4]) range_new[4] = k;
        if (k > range_new[5]) range_new[5] = k;
        }
      }
    }
  }

change = FALSE;
if (range[0] < range_new[0]) change = TRUE;
if (range[1] > range_new[1]) change = TRUE;
if (range[2] < range_new[2]) change = TRUE;
if (range[3] > range_new[3]) change = TRUE;
if (range[4] < range_new[4]) change = TRUE;
if (range[5] > range_new[5]) change = TRUE;

return change;
}

static void static_allocate_increase (int *range)
{
char *indices_old, *indices;
int *range_old;
int ii_old, jj_old, kk_old;
int ii, jj, kk;
int i, j, k;
int b_old, c_old;
int a, b, c;

indices_old = list.indices;
range_old = list.range;

/***************************************************
 * update indices: create new array, with all data *
 * initialized at 0, copy old data, free old array *
 ***************************************************/

a = range[1] - range[0] + 1;
b = range[3] - range[2] + 1;
c = range[5] - range[4] + 1;
indices = (char *) calloc (a * b * c, sizeof (char));

if (indices_old != NULL)
  {
  b_old = range_old[3] - range_old[2] + 1;
  c_old = range_old[5] - range_old[4] + 1;

  for (i = range_old[0]; i <= range_old[1]; i++)
    {
    ii_old = i - range_old[0];
    ii = i - range[0];

    for (j = range_old[2]; j <= range_old[3]; j++)
      {
      jj_old = j - range_old[2];
      jj = j - range[2];

      for (k = range_old[4]; k <= range_old[5]; k++)
        {
        kk_old = k - range_old[4];
        kk = k - range[4];

        indices[b * c * ii + c * jj + kk] = 
        indices_old[b_old * c_old * ii_old + c_old * jj_old + kk_old];
        }
      }
    }

  free (indices_old);
  }

list.indices = indices;

/****************
 * update range *
 ****************/

range_old[0] = range[0];
range_old[1] = range[1];
range_old[2] = range[2];
range_old[3] = range[3];
range_old[4] = range[4];
range_old[5] = range[5];
}

static void static_allocate_decrease (int *range)
{
char *indices_old, *indices;
int *range_old;
int ii_old, jj_old, kk_old;
int ii, jj, kk;
int i, j, k;
int b_old, c_old;
int a, b, c;

indices_old = list.indices;
range_old = list.range;

if (range[0] == INT_MAX && range[1] == -INT_MAX &&
    range[2] == INT_MAX && range[3] == -INT_MAX &&
    range[4] == INT_MAX && range[5] == -INT_MAX) indices = NULL;
else
  {
  /*******************************************************
   * update indices: copy relevant data to smaller array *
   *******************************************************/

  a = range[1] - range[0] + 1;
  b = range[3] - range[2] + 1;
  c = range[5] - range[4] + 1;
  indices = (char *) malloc (a * b * c * sizeof (char));

  b_old = range_old[3] - range_old[2] + 1;
  c_old = range_old[5] - range_old[4] + 1;

  for (i = range[0]; i <= range[1]; i++)
    {
    ii_old = i - range_old[0];
    ii = i - range[0];

    for (j = range[2]; j <= range[3]; j++)
      {
      jj_old = j - range_old[2];
      jj = j - range[2];

      for (k = range[4]; k <= range[5]; k++)
        {
        kk_old = k - range_old[4];
        kk = k - range[4];

        indices[b * c * ii + c * jj + kk] = 
        indices_old[b_old * c_old * ii_old + c_old * jj_old + kk_old];
        }
      }
    }
  }

free (indices_old);
list.indices = indices;

/****************
 * update range *
 ****************/

range_old[0] = range[0];
range_old[1] = range[1];
range_old[2] = range[2];
range_old[3] = range[3];
range_old[4] = range[4];
range_old[5] = range[5];
}

static void static_add (int *range_new, gamgi_bool value)
{
char *indices;
int *range;
int i, j, k;
int ii, jj, kk;
int b, c;

indices = list.indices;
range = list.range;

/****************
 * add new data *
 ****************/

b = range[3] - range[2] + 1;
c = range[5] - range[4] + 1;
for (i = range_new[0]; i <= range_new[1]; i++)
  {
  ii = i - range[0];
  for (j = range_new[2]; j <= range_new[3]; j++)
    {
    jj = j - range[2];
    for (k = range_new[4]; k <= range_new[5]; k++)
      {
      kk = k - range[4];
      indices[b * c * ii + c * jj + kk] = value;
      }
    }
  }

}

static gamgi_bool static_get (int *new, gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
const char *name;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, new + 0, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, new + 1, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, new + 2, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, new + 3, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, new + 4, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, new + 5, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

if (new[0] > new[1])
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

if (new[2] > new[3])
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

if (new[4] > new[5])
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

if (new[0] == 0 && new[1] == 0 && new[2] == 0 &&
new[3] == 0 && new[4] == 0 && new[5] == 0)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_update (int value, gamgi_window *window)
{
int range_new[6], range[6];

/**********************************************
 * import GTK data, return if data is invalid *
 **********************************************/

if (static_get (range_new, window) == FALSE) return FALSE;

/*******************************************************
 * if range changed, allocate/reallocate indices array *
 *******************************************************/

if (static_range_increase (range_new, range) == TRUE)
  static_allocate_increase (range);

/*********************************
 * add new data to indices array *
 *********************************/

static_add (range_new, value);

/***********************************************************
 * reduce array, if possible, after an Exclude instruction *
 ***********************************************************/

if (static_range_decrease (value, range_new) == TRUE)
  static_allocate_decrease (range_new);
  
return TRUE;
}

static void static_row (GtkListStore *store, int u, int v, int w)
{
GtkTreeIter iter;
char token[GAMGI_ENGINE_TOKEN];

sprintf (token, "[%d %d %d]", u, v, w);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, token, -1);
}

static void static_show (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *treeview;
GtkListStore *store;
char *indices;
int *range;
int i, j, k;
int ii, jj, kk;
int b, c;

indices = list.indices;
range = list.range;

/*****************************
 * add new store to treeview *
 *****************************/

store = gtk_list_store_new (1, G_TYPE_STRING);

if (indices != NULL)
  {
  b = range[3] - range[2] + 1;
  c = range[5] - range[4] + 1;

  for (i = range[1]; i >= range[0]; i--)
    {
    ii = i - range[0];
    for (j = range[3]; j >= range[2]; j--)
      {
      jj = j - range[2];
      for (k = range[5]; k >= range[4]; k--)
        {
        kk = k - range[4];

        if (i == 0 && j == 0 && k == 0) continue;
        if (indices[b * c * ii + c * jj + kk] == TRUE)
          static_row (store, i, j, k);
        }
      }
    }
  }

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "treeview");
gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);
}

static void static_hide (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u_min");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u_max");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v_min");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v_max");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w_min");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w_max");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_apply (int value, gamgi_window *window)
{
/*********************
 * update, show data *
 *********************/

if (static_update (value, window) == TRUE) static_show (window);
static_hide (window);
}

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data)
{
gamgi_gtk_direction_list_cancel (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

static void static_include (GtkWidget *widget, void *data)
{
static_apply (TRUE, GAMGI_CAST_WINDOW data);
}

static void static_exclude (GtkWidget *widget, void *data)
{
static_apply (FALSE, GAMGI_CAST_WINDOW data);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/**************************************
 * check for empty list of directions *
 **************************************/

if (list.indices == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction data", window);
  return;
  }

/***************************
 * keep data, close dialog *
 ***************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
g_signal_handlers_block_by_func (button, gamgi_gtk_direction_list, window);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
g_signal_handlers_unblock_by_func (button, gamgi_gtk_direction_list, window);

if (window->dialog1 != NULL)
  gamgi_gtk_dialog_task1_remove (widget, window);

gamgi_gtk_direction_create_uvw (FALSE, window);
}

static void static_create (gamgi_window *window)
{
GtkWidget *dialog;
GtkWidget *scrolled_window;
GtkWidget *frame;
GtkWidget *button;
GtkWidget *entry;
GtkWidget *label;
GtkWidget *table;
GtkWidget *vbox_dialog, *vbox_frame, *vbox_center, *vbox_top, *vbox_top_top;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;

dialog = gamgi_gtk_dialog_task1_create ("Direction List", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/********
 * List *
 ********/

vbox_frame = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_dialog), vbox_frame, TRUE, TRUE, 0);
gtk_container_set_border_width (GTK_CONTAINER (vbox_frame), 5);
gtk_widget_show (vbox_frame);

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_frame), frame, TRUE, TRUE, 0);
gtk_frame_set_label (GTK_FRAME (frame), " List ");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_container_add (GTK_CONTAINER (frame), hbox_center);
gtk_container_set_border_width (GTK_CONTAINER (hbox_center), 5);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, TRUE, TRUE, 0);
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
gtk_widget_set_size_request (treeview, 100, 150);
g_object_set_data (G_OBJECT (dialog), "treeview", treeview);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("List", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);

/********************
 * Include, Exclude *
 ********************/

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_center, FALSE, FALSE, 20);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Include");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_button_new_with_label ("<");
gtk_box_pack_start (GTK_BOX (vbox_top_top), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, 20, 25);
g_signal_connect (button, "clicked",
G_CALLBACK (static_include), window);
gtk_widget_show (button);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Exclude");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_button_new_with_label (">");
gtk_widget_set_size_request (button, 20, 25);
gtk_box_pack_end (GTK_BOX (vbox_top_top), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_exclude), window);
gtk_widget_show (button);

/*******************
 * direction range *
 *******************/

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_center, FALSE, FALSE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

label = gtk_label_new ("Max");
gtk_box_pack_start (GTK_BOX (vbox_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

table = gtk_table_new (3, 6, FALSE);
gtk_box_pack_start (GTK_BOX (vbox_top), table, FALSE, FALSE, 0);
gtk_widget_show (table);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_u_max", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_v_max", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_w_max", entry);
gtk_widget_show (entry);

label = gtk_label_new ("U");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("V");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 5);
gtk_widget_show (label);

label = gtk_label_new ("W");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 2, 3, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_u_min", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 2, 3, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_v_min", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 2, 3, GTK_EXPAND, GTK_EXPAND, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_w_min", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Min");
gtk_box_pack_start (GTK_BOX (vbox_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_direction_list_cancel), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}

static void static_direction (gamgi_direction *direction,
gamgi_cell *cell, gamgi_enum structure, gamgi_enum filter,
double thickness, gamgi_dlist *dlist, gamgi_enum pole, gamgi_enum trace)
{
if (cell->model == GAMGI_PHYS_PROJECTION)
  gamgi_phys_direction_projection (direction, cell, pole, trace);
else
  gamgi_phys_direction_node (direction,
  cell, structure, filter, thickness, dlist);
}

void gamgi_gtk_direction_list_create (gamgi_direction *direction,
gamgi_cell *cell, gamgi_enum structure, gamgi_enum filter, double thickness,
gamgi_dlist *dlist, gamgi_enum pole, gamgi_enum trace)
{
char *indices;
int *range;
int i, j, k;
int ii, jj, kk;
int b, c;

indices = list.indices;
range = list.range;

if (indices == NULL) static_direction (direction,
cell, structure, filter, thickness, dlist, pole, trace);
else
  {
  b = range[3] - range[2] + 1;
  c = range[5] - range[4] + 1;

  for (i = range[1]; i >= range[0]; i--)
    {
    ii = i - range[0];
    for (j = range[3]; j >= range[2]; j--)
      {
      jj = j - range[2];
      for (k = range[5]; k >= range[4]; k--)
        {
        kk = k - range[4];

        if (i == 0 && j == 0 && k == 0) continue;
        if (indices[b * c * ii + c * jj + kk] == TRUE)
          {
          direction->uvw[0] = i;
          direction->uvw[1] = j;
          direction->uvw[2] = k;
          static_direction (direction, cell, structure,
          filter, thickness, dlist, pole, trace);
          }
        }
      }
    }
  free (list.indices);
  list.indices = NULL;
  }
}

void gamgi_gtk_direction_list_start (gamgi_window *window)
{
list.indices = NULL;
list.range[0] = INT_MAX; list.range[1] = -INT_MAX;
list.range[2] = INT_MAX; list.range[3] = -INT_MAX;
list.range[4] = INT_MAX; list.range[5] = -INT_MAX;
}

void gamgi_gtk_direction_list_cancel (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

gamgi_gtk_direction_list (NULL, data);
}

void gamgi_gtk_direction_list_destroy (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

free (list.indices);
gamgi_gtk_direction_list_start (window);

if (window->dialog1 != NULL)
  gamgi_gtk_dialog_task1_remove (widget, window);
}

void gamgi_gtk_direction_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /******************************
   * create second level dialog *
   ******************************/

  static_create (window);
  static_show (window);
  }
else
  {
  /*******************************
   * free second level data      *
   * remove second level dialog  *
   * restart first level indices *
   *********************************/

  gamgi_gtk_direction_list_destroy (NULL, window);
  gamgi_gtk_direction_create_uvw (TRUE, window);
  }
}
