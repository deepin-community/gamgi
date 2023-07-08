/***********************************************
 *
 * $GAMGI/src/gtk/file/gamgi_gtk_file_export.h
 *
 * Copyright (C) 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_init (gamgi_window *window);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_file_export (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

GAMGI_GTK_FILE_EXPORT is called from the top window and creates the export
dialog. When users press ok, STATIC_OK is called, which tries to open the
file in append mode. If it fails, an error message dialog is called.
  Otherwise, it checks if the file existed before, in which case a warning
question dialog is called, asking users whether the old file should be
overwritten. The file is always closed before proceding.
  If the file did not exist previously or users want to overwrite, then
STATIC_WRITE is called and the file is open again, this time in write mode,
then written and finally closed.

 *************
 * Rationale *
 *************/
