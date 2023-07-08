/**********************************************
 *
 * $GAMGI/src/expat/gamgi_expat_import_help.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static gamgi_bool static_html_end (const char *element, gamgi_html *html);

static gamgi_bool static_html_start (const char *element, 
const char **attributes, gamgi_html *html);

static gamgi_bool static_head_end (const char *element, gamgi_html *html);

static gamgi_bool static_head_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_title_end (const char *element, gamgi_html *html);

static gamgi_bool static_title_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_link_end (const char *element, gamgi_html *html);

static gamgi_bool static_link_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_body_end (const char *element, gamgi_html *html);

static gamgi_bool static_body_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_div_end (const char *element, gamgi_html *html);

static gamgi_bool static_div_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_span_end (const char *element, gamgi_html *html);

static gamgi_bool static_span_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_ul_end (const char *element, gamgi_html *html);

static gamgi_bool static_ul_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_ol_end (const char *element, gamgi_html *html);

static gamgi_bool static_ol_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_li_end (const char *element, gamgi_html *html);

static gamgi_bool static_li_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_h1_end (const char *element, gamgi_html *html);

static gamgi_bool static_h1_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_h3_end (const char *element, gamgi_html *html);

static gamgi_bool static_h3_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_a_end (const char *element, gamgi_html *html);

static gamgi_bool static_a_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_img_end (const char *element, gamgi_html *html);

static gamgi_bool static_img_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_pre_end (const char *element, gamgi_html *html);

static gamgi_bool static_pre_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_p_end (const char *element, gamgi_html *html);

static gamgi_bool static_p_start (const char *element,
const char **attributes, gamgi_html *html);

static gamgi_bool static_b_end (const char *element, gamgi_html *html);

static gamgi_bool static_b_start (const char *element,
const char **attributes, gamgi_html *html);

 **********************
 * external functions *
 **********************/

void gamgi_expat_import_help_end (void *data, const char *element);

void gamgi_expat_import_help_start (void *data, const char *element,
const char **attributes);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
