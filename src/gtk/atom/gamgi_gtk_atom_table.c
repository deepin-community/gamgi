/**********************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_table.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_chem.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_chem_atom.h"
#include "gamgi_io_token.h"

static void static_clicked (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
int element;

element = GAMGI_POINTER_TO_INT (g_object_get_data (G_OBJECT (widget), "button_element"));
sprintf (token, "%d", element);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_button (GtkWidget *table, int element, 
int x0, int  y0, int x1, int y1, 
gamgi_window *window)
{
char token[GAMGI_ENGINE_TOKEN];
GtkWidget *button;
GdkColor color;

gamgi_chem_atom_name (element, token);

gamgi_gtk_dialog_color (&color, gamgi->atom->red[element], 
gamgi->atom->green[element], gamgi->atom->blue[element]);

/**************
 * foreground *
 **************/

if (color.red + color.green + color.blue > 3*65535/2)
  button = gamgi_gtk_dialog_button_create (token, "black");
else
  button = gamgi_gtk_dialog_button_create (token, "white");

/**************
 * background *
 **************/

gamgi_gtk_dialog_color_bg (button, color);

gtk_table_attach_defaults (GTK_TABLE (table), button, x0, y0, x1, y1);
g_object_set_data (G_OBJECT (button), "button_element", GAMGI_INT_TO_POINTER (element));
g_signal_connect (button, "clicked",
G_CALLBACK (static_clicked), window);
gtk_widget_show (button);
}

static void static_remove (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_table");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
}

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data)
{
static_remove (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

void gamgi_gtk_atom_table (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox;
GtkWidget *hbox;
GtkWidget *table;
GtkWidget *button;

dialog = gamgi_gtk_dialog_task1_create ("Periodic Table", window);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox = gtk_vbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (dialog), vbox);
gtk_widget_show (vbox);

table = gtk_table_new (9, 18, TRUE);
gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);

static_button (table, GAMGI_CHEM_H, 0, 1, 0, 1, window);
static_button (table, GAMGI_CHEM_HE, 17, 18, 0, 1, window);

static_button (table, GAMGI_CHEM_LI, 0, 1, 1, 2, window);
static_button (table, GAMGI_CHEM_BE, 1, 2, 1, 2, window);
static_button (table, GAMGI_CHEM_B, 12, 13, 1, 2, window);
static_button (table, GAMGI_CHEM_C, 13, 14, 1, 2, window);
static_button (table, GAMGI_CHEM_N, 14, 15, 1, 2, window);
static_button (table, GAMGI_CHEM_O, 15, 16, 1, 2, window);
static_button (table, GAMGI_CHEM_F, 16, 17, 1, 2, window);
static_button (table, GAMGI_CHEM_NE, 17, 18, 1, 2, window);

static_button (table, GAMGI_CHEM_NA, 0, 1, 2, 3, window);
static_button (table, GAMGI_CHEM_MG, 1, 2, 2, 3, window);
static_button (table, GAMGI_CHEM_AL, 12, 13, 2, 3, window);
static_button (table, GAMGI_CHEM_SI, 13, 14, 2, 3, window);
static_button (table, GAMGI_CHEM_P, 14, 15, 2, 3, window);
static_button (table, GAMGI_CHEM_S, 15, 16, 2, 3, window);
static_button (table, GAMGI_CHEM_CL, 16, 17, 2, 3, window);
static_button (table, GAMGI_CHEM_AR, 17, 18, 2, 3, window);

static_button (table, GAMGI_CHEM_K, 0, 1, 3, 4, window);
static_button (table, GAMGI_CHEM_CA, 1, 2, 3, 4, window);
static_button (table, GAMGI_CHEM_SC, 2, 3, 3, 4, window);
static_button (table, GAMGI_CHEM_TI, 3, 4, 3, 4, window);
static_button (table, GAMGI_CHEM_V, 4, 5, 3, 4, window);
static_button (table, GAMGI_CHEM_CR, 5, 6, 3, 4, window);
static_button (table, GAMGI_CHEM_MN, 6, 7, 3, 4, window);
static_button (table, GAMGI_CHEM_FE, 7, 8, 3, 4, window);
static_button (table, GAMGI_CHEM_CO, 8, 9, 3, 4, window);
static_button (table, GAMGI_CHEM_NI, 9, 10, 3, 4, window);
static_button (table, GAMGI_CHEM_CU, 10, 11, 3, 4, window);
static_button (table, GAMGI_CHEM_ZN, 11, 12, 3, 4, window);
static_button (table, GAMGI_CHEM_GA, 12, 13, 3, 4, window);
static_button (table, GAMGI_CHEM_GE, 13, 14, 3, 4, window);
static_button (table, GAMGI_CHEM_AS, 14, 15, 3, 4, window);
static_button (table, GAMGI_CHEM_SE, 15, 16, 3, 4, window);
static_button (table, GAMGI_CHEM_BR, 16, 17, 3, 4, window);
static_button (table, GAMGI_CHEM_KR, 17, 18, 3, 4, window);

static_button (table, GAMGI_CHEM_RB, 0, 1, 4, 5, window);
static_button (table, GAMGI_CHEM_SR, 1, 2, 4, 5, window);
static_button (table, GAMGI_CHEM_Y, 2, 3, 4, 5, window);
static_button (table, GAMGI_CHEM_ZR, 3, 4, 4, 5, window);
static_button (table, GAMGI_CHEM_NB, 4, 5, 4, 5, window);
static_button (table, GAMGI_CHEM_MO, 5, 6, 4, 5, window);
static_button (table, GAMGI_CHEM_TC, 6, 7, 4, 5, window);
static_button (table, GAMGI_CHEM_RU, 7, 8, 4, 5, window);
static_button (table, GAMGI_CHEM_RH, 8, 9, 4, 5, window);
static_button (table, GAMGI_CHEM_PD, 9, 10, 4, 5, window);
static_button (table, GAMGI_CHEM_AG, 10, 11, 4, 5, window);
static_button (table, GAMGI_CHEM_CD, 11, 12, 4, 5, window);
static_button (table, GAMGI_CHEM_IN, 12, 13, 4, 5, window);
static_button (table, GAMGI_CHEM_SN, 13, 14, 4, 5, window);
static_button (table, GAMGI_CHEM_SB, 14, 15, 4, 5, window);
static_button (table, GAMGI_CHEM_TE, 15, 16, 4, 5, window);
static_button (table, GAMGI_CHEM_I, 16, 17, 4, 5, window);
static_button (table, GAMGI_CHEM_XE, 17, 18, 4, 5, window);

static_button (table, GAMGI_CHEM_CS, 0, 1, 5, 6, window);
static_button (table, GAMGI_CHEM_BA, 1, 2, 5, 6, window);
static_button (table, GAMGI_CHEM_HF, 3, 4, 5, 6, window);
static_button (table, GAMGI_CHEM_TA, 4, 5, 5, 6, window);
static_button (table, GAMGI_CHEM_W, 5, 6, 5, 6, window);
static_button (table, GAMGI_CHEM_RE, 6, 7, 5, 6, window);
static_button (table, GAMGI_CHEM_OS, 7, 8, 5, 6, window);
static_button (table, GAMGI_CHEM_IR, 8, 9, 5, 6, window);
static_button (table, GAMGI_CHEM_PT, 9, 10, 5, 6, window);
static_button (table, GAMGI_CHEM_AU, 10, 11, 5, 6, window);
static_button (table, GAMGI_CHEM_HG, 11, 12, 5, 6, window);
static_button (table, GAMGI_CHEM_TL, 12, 13, 5, 6, window);
static_button (table, GAMGI_CHEM_PB, 13, 14, 5, 6, window);
static_button (table, GAMGI_CHEM_BI, 14, 15, 5, 6, window);
static_button (table, GAMGI_CHEM_PO, 15, 16, 5, 6, window);
static_button (table, GAMGI_CHEM_AT, 16, 17, 5, 6, window);
static_button (table, GAMGI_CHEM_RN, 17, 18, 5, 6, window);

static_button (table, GAMGI_CHEM_FR, 0, 1, 6, 7, window);
static_button (table, GAMGI_CHEM_RA, 1, 2, 6, 7, window);
static_button (table, GAMGI_CHEM_RF, 3, 4, 6, 7, window);
static_button (table, GAMGI_CHEM_DB, 4, 5, 6, 7, window);
static_button (table, GAMGI_CHEM_SG, 5, 6, 6, 7, window);
static_button (table, GAMGI_CHEM_BH, 6, 7, 6, 7, window);
static_button (table, GAMGI_CHEM_HS, 7, 8, 6, 7, window);
static_button (table, GAMGI_CHEM_MT, 8, 9, 6, 7, window);
static_button (table, GAMGI_CHEM_DS, 9, 10, 6, 7, window);
static_button (table, GAMGI_CHEM_RG, 10, 11, 6, 7, window);

static_button (table, GAMGI_CHEM_LA, 2, 3, 7, 8, window);
static_button (table, GAMGI_CHEM_CE, 3, 4, 7, 8, window);
static_button (table, GAMGI_CHEM_PR, 4, 5, 7, 8, window);
static_button (table, GAMGI_CHEM_ND, 5, 6, 7, 8, window);
static_button (table, GAMGI_CHEM_PM, 6, 7, 7, 8, window);
static_button (table, GAMGI_CHEM_SM, 7, 8, 7, 8, window);
static_button (table, GAMGI_CHEM_EU, 8, 9, 7, 8, window);
static_button (table, GAMGI_CHEM_GD, 9, 10, 7, 8, window);
static_button (table, GAMGI_CHEM_TB, 10, 11, 7, 8, window);
static_button (table, GAMGI_CHEM_DY, 11, 12, 7, 8, window);
static_button (table, GAMGI_CHEM_HO, 12, 13, 7, 8, window);
static_button (table, GAMGI_CHEM_ER, 13, 14, 7, 8, window);
static_button (table, GAMGI_CHEM_TM, 14, 15, 7, 8, window);
static_button (table, GAMGI_CHEM_YB, 15, 16, 7, 8, window);
static_button (table, GAMGI_CHEM_LU, 16, 17, 7, 8, window);

static_button (table, GAMGI_CHEM_AC, 2, 3, 8, 9, window);
static_button (table, GAMGI_CHEM_TH, 3, 4, 8, 9, window);
static_button (table, GAMGI_CHEM_PA, 4, 5, 8, 9, window);
static_button (table, GAMGI_CHEM_U, 5, 6, 8, 9, window);
static_button (table, GAMGI_CHEM_NP, 6, 7, 8, 9, window);
static_button (table, GAMGI_CHEM_PU, 7, 8, 8, 9, window);
static_button (table, GAMGI_CHEM_AM, 8, 9, 8, 9, window);
static_button (table, GAMGI_CHEM_CM, 9, 10, 8, 9, window);
static_button (table, GAMGI_CHEM_BK, 10, 11, 8, 9, window);
static_button (table, GAMGI_CHEM_CF, 11, 12, 8, 9, window);
static_button (table, GAMGI_CHEM_ES, 12, 13, 8, 9, window);
static_button (table, GAMGI_CHEM_FM, 13, 14, 8, 9, window);
static_button (table, GAMGI_CHEM_MD, 14, 15, 8, 9, window);
static_button (table, GAMGI_CHEM_NO, 15, 16, 8, 9, window);
static_button (table, GAMGI_CHEM_LR, 16, 17, 8, 9, window);
gtk_widget_show (table);

hbox = gtk_hbox_new (FALSE, 0);
gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 5);
gtk_widget_show (hbox);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}
