/**********************************************
 *
 * $GAMGI/src/expat/gamgi_expat_import_help.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_expat.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_io_token.h"
#include "gamgi_io_file.h"
#include "gamgi_io_error.h"
#include "gamgi_expat_import.h"

static gamgi_bool static_html_end (const char *element, gamgi_html *html)
{
/************************************************
 * parent stack ends here: output parent = NULL *
 ************************************************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_html_start (const char *element, 
const char **attributes, gamgi_html *html)
{
char string[GAMGI_ENGINE_LINE];
int fileline, i;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/************************************
 * html should be the first element *
 ************************************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_HTML, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/****************************
 * parent stack starts here *
 ****************************/

html->ml.parent = gamgi_engine_slist_add_start (NULL);
html->ml.parent->data = (void *) GAMGI_EXPAT_HTML;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /******************************
   * namespace (XHTML optional) *
   ******************************/

  if (strcmp (attributes[i], "xmlns") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_LINE) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /*****************************
   * language (XHTML optional) *
   *****************************/

  else if (strcmp (attributes[i], "lang") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /*********************************
   * reject other attributes, even * 
   * if they are accepted by XHTML *
   *********************************/
 
  else return gamgi_io_error_attribute (attributes[i], 
  html->ml.filename, fileline, html->ml.window);
  }

return html->ml.valid;
}

static gamgi_bool static_head_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_head_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_HEAD, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_HEAD;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

return html->ml.valid;
}

static gamgi_bool static_title_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_title_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_TITLE, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_TITLE;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

return html->ml.valid;
}

static gamgi_bool static_link_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_link_start (const char *element,
const char **attributes, gamgi_html *html)
{
char string[GAMGI_ENGINE_LINE];
int fileline, i;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_LINK, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_LINK;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /**********************
   * rel (CSS required) *
   **********************/

  if (strcmp (attributes[i], "rel") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);

    /**********************
     * stylesheet or icon *
     **********************/

    if (strcmp (attributes[i + 1], "stylesheet") != 0
    && strcmp (attributes[i + 1], "icon") != 0)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /***********************
   * type (CSS required) *
   ***********************/

  else if (strcmp (attributes[i], "type") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    
    /*************************
     * text/css or image/png *
     *************************/

    if (strcmp (attributes[i + 1], "text/css") != 0
    && strcmp (attributes[i + 1], "image/png") != 0)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /***********************
   * href (CSS optional) *
   ***********************/

  else if (strcmp (attributes[i], "href") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_LINE) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);

    /****************************************
     * GAMGI does not use this file, so it  *
     * checks the filename but not the file *
     ****************************************/

    if (gamgi_io_file_name (attributes[i + 1], html->ml.window) == NULL)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /*********************************
   * reject other attributes, even *
   * if they are accepted by XHTML *
   *********************************/

  else return gamgi_io_error_attribute (attributes[i], 
  html->ml.filename, fileline, html->ml.window);
  }

return html->ml.valid;
}

static gamgi_bool static_body_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_body_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_BODY, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_BODY;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

return html->ml.valid;
}

static gamgi_bool static_div_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

/************************************
 * signal the end of contents:      *
 * Help dialogs show character data *
 * only when html->contents is TRUE *
 ************************************/

html->contents = FALSE;

return html->ml.valid;
}

static gamgi_bool static_div_start (const char *element,
const char **attributes, gamgi_html *html)
{
char string[GAMGI_ENGINE_TOKEN];
int fileline, i;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_DIV, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_DIV;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /***********************
   * id (XHTML optional) *
   ***********************/

  if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      html->ml.filename, fileline, html->ml.window);

    /*************************************
     * values defined in $GAMGI/doc/css/ *
     *************************************/

    if (strcmp (attributes[i + 1], "notebook") == 0) continue;
    if (strcmp (attributes[i + 1], "bottom") == 0) continue;

    else return gamgi_io_error_value (attributes[i + 1],
    html->ml.filename, fileline, html->ml.window);
    }

  /**************************
   * class (XHTML optional) *
   **************************/

  else if (strcmp (attributes[i], "class") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      html->ml.filename, fileline, html->ml.window);

    /*************************************
     * values defined in $GAMGI/doc/css/ *
     *************************************/

    if (strcmp (attributes[i + 1], "board") == 0) continue;

    /************************************
     * signal the start of contents:    *
     * Help dialogs show character data *
     * only when html->contents is TRUE *
     ************************************/

    if (strcmp (attributes[i + 1], "contents") == 0) html->contents = TRUE;

    else return gamgi_io_error_value (attributes[i + 1],
    html->ml.filename, fileline, html->ml.window);
    }

  /*********************************
   * reject other attributes, even *
   * if they are accepted by XHTML *
   *********************************/

  else return gamgi_io_error_attribute (attributes[i], 
  html->ml.filename, fileline, html->ml.window);
  }

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_normal");
strcpy (html->color, "color_text");

return html->ml.valid;
}

static gamgi_bool static_span_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_span_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_SPAN, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_SPAN;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0],
  html->ml.filename, fileline, html->ml.window);

return html->ml.valid;
}

static gamgi_bool static_ul_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_ul_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_UL, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_UL;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

return html->ml.valid;
}

static gamgi_bool static_ol_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_ol_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_OL, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_OL;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

return html->ml.valid;
}

static gamgi_bool static_li_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

/*****************
 * add 1 newline *
 *****************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n", 1, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_li_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_LI, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_LI;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

/******************
 * add 2 newlines *
 ******************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n\n", 2, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_a_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_normal");
strcpy (html->color, "color_text");

return html->ml.valid;
}

static gamgi_bool static_a_start (const char *element,
const char **attributes, gamgi_html *html)
{
char string[GAMGI_ENGINE_LINE];
int fileline, i;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_A, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_A;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*************************
   * href (XHTML required) *
   *************************/

  if (strcmp (attributes[i], "href") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_LINE) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

   /************************
   * name (XHTML optional) *
   *************************/

  else if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /************************
   * class (CSS optional) *
   ************************/

  else if (strcmp (attributes[i], "class") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /*********************************
   * reject other attributes, even *
   * if they are accepted by XHTML *
   *********************************/

  else return gamgi_io_error_attribute (attributes[i], 
  html->ml.filename, fileline, html->ml.window);
  }

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_normal");
strcpy (html->color, "color_link");

return html->ml.valid;
}

static gamgi_bool static_img_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_img_start (const char *element,
const char **attributes, gamgi_html *html)
{
char string[GAMGI_ENGINE_LINE];
int fileline, i;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_IMG, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_IMG;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /************************
   * src (XHTML required) *
   ************************/

  if (strcmp (attributes[i], "src") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_LINE) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /************************
   * alt (XHTML required) *
   ************************/

  else if (strcmp (attributes[i], "alt") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /**************************
   * title (XHTML optional) *
   **************************/

  else if (strcmp (attributes[i], "title") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /**************************
   * width (XHTML optional) *
   **************************/

  else if (strcmp (attributes[i], "width") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /***************************
   * height (XHTML optional) *
   ***************************/

  else if (strcmp (attributes[i], "height") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /************************
   * class (CSS optional) *
   ************************/

  else if (strcmp (attributes[i], "class") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /*********************************
   * reject other attributes, even *
   * if they are accepted by XHTML *
   *********************************/

  else return gamgi_io_error_attribute (attributes[i], 
  html->ml.filename, fileline, html->ml.window);
  }

return html->ml.valid;
}

static gamgi_bool static_pre_end (const char *element, gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

/*********************
 * turn preserve off *
 *********************/

html->preserve = FALSE;

/*****************
 * add 1 newline *
 *****************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n", 1, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_pre_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_PRE, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_PRE;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

/********************
 * turn preserve on *
 ********************/

html->preserve = TRUE;

/*****************
 * add 1 newline *
 *****************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n", 1, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_br_end (const char *element,
gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_br_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_BR, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_BR;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

/*****************
 * add 1 newline *
 *****************/

gtk_text_buffer_insert_with_tags_by_name (html->buffer,
&html->iter, "\n", 1, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_p_end (const char *element,
gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

return html->ml.valid;
}

static gamgi_bool static_p_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_P, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_P;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

/******************
 * add 2 newlines *
 ******************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n\n", 2, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_h1_end (const char *element,
gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_normal");
strcpy (html->color, "color_text");

/*****************
 * add 1 newline *
 *****************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n", 1, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_h1_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_H1, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename,
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_H1;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0],
  html->ml.filename, fileline, html->ml.window);

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_large");
strcpy (html->color, "color_title");

/******************
 * add 2 newlines *
 ******************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n\n", 2, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_h3_end (const char *element,
gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_normal");
strcpy (html->color, "color_text");

/*****************
 * add 1 newline *
 *****************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n", 1, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_h3_start (const char *element,
const char **attributes, gamgi_html *html)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_H3, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_H3;

/***************************************
 * reject attributes for this element, *
 * even if they are accepted by XHTML  *
 ***************************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0], 
  html->ml.filename, fileline, html->ml.window);

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_large");
strcpy (html->color, "color_title");

/******************
 * add 2 newlines *
 ******************/

if (html->contents == TRUE)
  gtk_text_buffer_insert_with_tags_by_name (html->buffer,
  &html->iter, "\n\n", 2, html->size, html->color, NULL);

return html->ml.valid;
}

static gamgi_bool static_b_end (const char *element,
gamgi_html *html)
{
/**********************
 * update parent list *
 **********************/

html->ml.parent = gamgi_engine_slist_remove_start (html->ml.parent);

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_normal");
strcpy (html->color, "color_text");

return html->ml.valid;
}

static gamgi_bool static_b_start (const char *element,
const char **attributes, gamgi_html *html)
{
char string[GAMGI_ENGINE_TOKEN];
int fileline, i;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_html (GAMGI_EXPAT_B, html) == FALSE)
  return gamgi_io_error_child (html->ml.filename, 
  fileline, html->ml.window);

/************************
 * parent stack: update *
 ************************/

html->ml.parent = gamgi_engine_slist_add_start (html->ml.parent);
html->ml.parent->data = (void *) GAMGI_EXPAT_B;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /************************
   * class (CSS optional) *
   ************************/

  if (strcmp (attributes[i], "class") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    string, GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      html->ml.filename, fileline, html->ml.window);
    }

  /*********************************
   * reject other attributes, even *
   * if they are accepted by XHTML *
   *********************************/

  else return gamgi_io_error_attribute (attributes[i], 
  html->ml.filename, fileline, html->ml.window);
  }

/***************************
 * set text size and color *
 ***************************/

strcpy (html->size, "size_normal");
strcpy (html->color, "color_bold");

return html->ml.valid;
}

void gamgi_expat_import_help_end (void *data,
const char *element)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

if (strcmp (element, "html") == 0)
  ml->valid = static_html_end (element, data);

else if (strcmp (element, "head") == 0)
  ml->valid = static_head_end (element, data);

else if (strcmp (element, "title") == 0)
  ml->valid = static_title_end (element, data);

else if (strcmp (element, "link") == 0)
  ml->valid = static_link_end (element, data);

else if (strcmp (element, "body") == 0)
  ml->valid = static_body_end (element, data);

else if (strcmp (element, "div") == 0)
  ml->valid = static_div_end (element, data);

else if (strcmp (element, "span") == 0)
  ml->valid = static_span_end (element, data);

else if (strcmp (element, "ul") == 0)
  ml->valid = static_ul_end (element, data);

else if (strcmp (element, "ol") == 0)
  ml->valid = static_ol_end (element, data);

else if (strcmp (element, "li") == 0)
  ml->valid = static_li_end (element, data);

else if (strcmp (element, "a") == 0)
  ml->valid = static_a_end (element, data);

else if (strcmp (element, "img") == 0)
  ml->valid = static_img_end (element, data);

else if (strcmp (element, "pre") == 0)
  ml->valid = static_pre_end (element, data);

else if (strcmp (element, "br") == 0)
  ml->valid = static_br_end (element, data);

else if (strcmp (element, "p") == 0)
  ml->valid = static_p_end (element, data);

else if (strcmp (element, "h1") == 0)
  ml->valid = static_h1_end (element, data);

else if (strcmp (element, "h3") == 0)
  ml->valid = static_h3_end (element, data);

else if (strcmp (element, "b") == 0)
  ml->valid = static_b_end (element, data);

else ml->valid = gamgi_io_error_element (element, 
ml->filename, fileline, ml->window);

if (ml->valid == FALSE) XML_SetElementHandler (ml->parser, NULL, NULL);
}

void gamgi_expat_import_help_start (void *data,
const char *element, const char **attributes)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

if (strcmp (element, "html") == 0)
  ml->valid = static_html_start (element, attributes, data);

else if (strcmp (element, "head") == 0)
  ml->valid = static_head_start (element, attributes, data);

else if (strcmp (element, "title") == 0)
  ml->valid = static_title_start (element, attributes, data);

else if (strcmp (element, "link") == 0)
  ml->valid = static_link_start (element, attributes, data);

else if (strcmp (element, "body") == 0)
  ml->valid = static_body_start (element, attributes, data);

else if (strcmp (element, "div") == 0)
  ml->valid = static_div_start (element, attributes, data);

else if (strcmp (element, "span") == 0)
  ml->valid = static_span_start (element, attributes, data);

else if (strcmp (element, "ul") == 0)
  ml->valid = static_ul_start (element, attributes, data);

else if (strcmp (element, "ol") == 0)
  ml->valid = static_ol_start (element, attributes, data);

else if (strcmp (element, "li") == 0)
  ml->valid = static_li_start (element, attributes, data);

else if (strcmp (element, "a") == 0)
  ml->valid = static_a_start (element, attributes, data);

else if (strcmp (element, "img") == 0)
  ml->valid = static_img_start (element, attributes, data);

else if (strcmp (element, "pre") == 0)
  ml->valid = static_pre_start (element, attributes, data);

else if (strcmp (element, "br") == 0)
  ml->valid = static_br_start (element, attributes, data);

else if (strcmp (element, "p") == 0)
  ml->valid = static_p_start (element, attributes, data);

else if (strcmp (element, "h1") == 0)
  ml->valid = static_h1_start (element, attributes, data);

else if (strcmp (element, "h3") == 0)
  ml->valid = static_h3_start (element, attributes, data);

else if (strcmp (element, "b") == 0)
  ml->valid = static_b_start (element, attributes, data);

else ml->valid = gamgi_io_error_element (element, 
ml->filename, fileline, ml->window);

/**********************************
 * disable Expat parser callbacks *
 **********************************/

if (ml->valid == FALSE)
  XML_SetElementHandler (ml->parser, NULL, NULL);
}
