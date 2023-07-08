/*******************************************
 *
 * $GAMGI/src/io/gamgi_io_token.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

void gamgi_io_token_cut (const char *string, char *token, int length);

void gamgi_io_token_clean (const char *string, char *token);

void gamgi_io_token_lower (const char *token, char *lower);

gamgi_bool gamgi_io_token_size_check (const char *token, int length);

gamgi_bool gamgi_io_token_check (const char *string);

gamgi_bool gamgi_io_token_alpha_check (const char *token, 
const char *valid, const int length);

void gamgi_io_token_bool_get (const char *token, gamgi_bool *bool);

void gamgi_io_token_long_get (const char *token, long *integer);

void gamgi_io_token_int_get (const char *token, int *integer);

void gamgi_io_token_double_get (const char *token, double *real);

void gamgi_io_token_float_get (const char *token, float *real);

gamgi_bool gamgi_io_token_long_get_check (const char *token, 
long *integer, const long min, const long max);

gamgi_bool gamgi_io_token_int_get_check (const char *token,
int *integer, const int min, const int max);

gamgi_bool gamgi_io_token_double_get_check (const char *token, 
double *real, const double min, const double max);

gamgi_bool gamgi_io_token_float_get_check (const char *token,
float *real, const float min, const float max);

gamgi_bool gamgi_io_token_alpha_scan (const char *string, 
char *alpha, const char *valid, const int length);

gamgi_bool gamgi_io_token_bool_scan (const char *string, gamgi_bool *bool);

gamgi_bool gamgi_io_token_long_scan (const char *string, 
long *integer, const long min, const long max);

gamgi_bool gamgi_io_token_int_scan (const char *string,
int *integer, const int min, const int max);

gamgi_bool gamgi_io_token_double_scan (const char *string, 
double *real, const double min, const double max);

gamgi_bool gamgi_io_token_float_scan (const char *string,
float *real, const float min, const float max);

gamgi_bool gamgi_io_token_alpha_long_scan (const char *string, 
char *alpha, long *integer, const int length, 
const long min, const long max);

void gamgi_io_token_remove (char **new);

void gamgi_io_token_create (const char *old, char **new);

gamgi_bool gamgi_io_token_check_create (const char *old, char **new, const int max);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
