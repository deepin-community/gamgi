/*******************************************
 *
 * $GAMGI/src/io/gamgi_io_token.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_io.h"

#include <errno.h>
#include <ctype.h>

/************* external function ***********
 *                                         *
 *             GAMGI_IO_TOKEN_CUT          *
 *                                         *
 *******************************************/

void gamgi_io_token_cut (const char *string, char *token, int length)
{
/********************************************************
 * length is the true output array length, so the       *
 * maximum number of characters that can be effectively *
 * copied is length - 1, to save the last position to   *
 * the end character: output[length - 1] = '\0';        *
 *                                                      *
 * standard strncpy function truncates strings with the *
 * maximum size (length - 1) without ending the string, *
 * so adding the terminating '\0' character is required *
 ********************************************************/

strncpy (token, string, length - 1);
token[length - 1] = '\0';
}

/************* external function ***********
 *                                         *
 *           GAMGI_IO_TOKEN_CLEAN          *
 *                                         *
 * Remove all delimiter char around string *
 *******************************************/

void gamgi_io_token_clean (const char *string, char *token)
{
char *delimiters = GAMGI_IO_DELIMITERS;
char buffer[GAMGI_ENGINE_LINE];

strcpy (buffer, string);
strcpy (token, strtok (buffer, delimiters));
}

/************** external function ************
 *                                           *
 *            GAMGI_IO_TOKEN_LOWER           *
 *                                           *
 * Convert all char in a token to lower case *
 *********************************************/

void gamgi_io_token_lower (const char *token, char *lower)
{
while (*token != '\0') *lower++ = tolower(*token++);
*lower = '\0';
}

/****************** external function ***************
 *                                                  *
 *             GAMGI_IO_TOKEN_SIZE_CHECK            *
 *                                                  *
 * Check if the token length is larger than length  *
 *  and return the result in a flag TRUE/FALSE.     *
 ****************************************************/

gamgi_bool gamgi_io_token_size_check (const char *token, int length)
{
/***********************************************
 * Check whether token is larger than max char *
 * To disable this test make length = INT_MAX. *
 ***********************************************/

if (strlen (token) > length - 1) return FALSE;

return TRUE;
}

/*********** external function ***********
 *                                       *
 *          GAMGI_IO_TOKEN_CHECK         *
 *                                       *
 * Check string contents:                *
 * 1) return FALSE when only delimiter   *
 * char or no char at all were found.    *
 * 2) return TRUE when something other   *
 * than delimiter char were found.       *
 * This function does NOT check for good *
 * or bad tokens, only if they exist!    *
 *****************************************/

gamgi_bool gamgi_io_token_check (const char *string)
{
char *delimiters = GAMGI_IO_DELIMITERS;
char buffer[GAMGI_ENGINE_LINE];

/****************************************************
 * If string has more char than GAMGI_ENGINE_LINE,  *
 * then something is wrong. Passing TRUE means that *
 * something is there, which should trigger a scan  *
 * function, which will report the error.           *
 ****************************************************/

if (gamgi_io_token_size_check (string, GAMGI_ENGINE_LINE) == FALSE) return TRUE;

/*************************************
 * strtok is a destructive function, *
 * thus use a buffer to save string. *
 *************************************/

strcpy (buffer, string);
if (strtok (buffer, delimiters) == NULL) return FALSE;
else return TRUE;
}

/****************** external function ************
 *                                               *
 *            GAMGI_IO_TOKEN_ALPHA_CHECK         *
 *                                               *
 * Check:                                        *
 * 1) if the token length is larger than length; *
 * 2) if the token has non accepted char;        *
 * and return the result in a flag TRUE/FALSE.   *
 *************************************************/

gamgi_bool gamgi_io_token_alpha_check (const char *token, 
const char *valid, const int length)
{
/*********************************************************
 * Argument valid lists every char that can be present   *
 * in argument token. As strspn () returns the number of *
 * skipped char until an unlisted char is found, in a    *
 * valid token this number must equal the token length.  *
 *********************************************************/

if (strspn (token, valid) != strlen (token)) return FALSE;

/***********************************************
 * Check whether token is larger than max char *
 ***********************************************/

if (gamgi_io_token_size_check (token, length) == FALSE) return FALSE;

return TRUE;
}

/************** external function ************
 *                                           *
 *           GAMGI_IO_TOKEN_BOOL_GET         *
 *                                           *
 * Convert a token to bool without checking. *
 *********************************************/

void gamgi_io_token_bool_get (const char *token, gamgi_bool *bool)
{
if (strcmp (token, "yes") == 0) *bool = TRUE;
else if (strcmp (token, "no") == 0) *bool = FALSE;
}

/**************** external function **********
 *                                           *
 *             GAMGI_IO_TOKEN_LONG_GET       *
 *                                           *
 * Convert a token to long without checking. *
 *********************************************/

void gamgi_io_token_long_get (const char *token, long *integer)
{
char *end;

*integer = strtol (token, &end, 10);
}

/*************** external function **********
 *                                          *
 *             GAMGI_IO_TOKEN_INT_GET       *
 *                                          *
 * Convert a token to int without checking. *
 *********************************************/

void gamgi_io_token_integer_get (const char *token, int *integer)
{
char *end;

*integer = (int) strtol (token, &end, 10);
}

/**************** external function ************
 *                                             *
 *            GAMGI_IO_TOKEN_DOUBLE_GET        *
 *                                             *
 * Convert a token to double without checking. *
 ***********************************************/

void gamgi_io_token_double_get (const char *token, double *real)
{
char *end;

*real = strtod (token, &end);
}

/*************** external function ************
 *                                            *
 *            GAMGI_IO_TOKEN_FLOAT_GET        *
 *                                            *
 * Convert a token to float without checking. *
 ***********************************************/

void gamgi_io_token_float_get (const char *token, float *real)
{
char *end;

*real = (float) strtod (token, &end);
}

/*********** external function ********
 *                                    *
 *   GAMGI_IO_TOKEN_LONG_GET_CHECK    *
 *                                    *
 * Convert a token to long, checking: *
 * 1) if it contains wrong char;      *
 * 2) if it is out of range;          *
 * 3) if it is smaller than min;      *
 * 4) if it is larger than max;       *
 **************************************/

gamgi_bool gamgi_io_token_long_get_check (const char *token, 
long *integer, const long min, const long max)
{
char *end;

/*****************************************************
 * Reset errno, the error variable. This is required *
 * because some C functions set errno when things go *
 * wrong but don't set errno when things go right.   *
 *****************************************************/

errno = 0;

*integer = strtol (token, &end, 10);

/*****************************************************
 * Check to guarantee that the input token is clean. *
 * If a invalid char is found then strtol () stops   *
 * and *end contains the faulty char, otherwise it   *
 * will contain the '\0' char provided by strtok (). *
 *****************************************************/

if (*end != '\0') return FALSE;

/********************************************************
 *  Check to guarantee that the converted integer is    *
 * inside the range of values accepted by the compiler. *
 ********************************************************/

if (errno == ERANGE) return FALSE;

/**************************************************
 * Check to guarantee that the converted integer  *
 *  is not smaller than min or larger than max.   *
 *  To disable these tests, make min = -LONG_MAX  *
 *  or max = LONG_MAX when calling this function. *
 **************************************************/
   
if (*integer < min) return FALSE;
if (*integer > max) return FALSE;

return TRUE;
}

/*********** external function ********
 *                                    *
 *    GAMGI_IO_TOKEN_INT_GET_CHECK    *
 *                                    *
 * Convert a token to int, checking:  *
 * 1) if it contains wrong char;      *
 * 2) if it is out of range;          *
 * 3) if it is smaller than min;      *
 * 4) if it is larger than max;       *
 **************************************/

gamgi_bool gamgi_io_token_int_get_check (const char *token, 
int *integer, const int min, const int max)
{
char *end;

/*****************************************************
 * Reset errno, the error variable. This is required *
 * because some C functions set errno when things go *
 * wrong but don't set errno when things go right.   *
 *****************************************************/

errno = 0;

*integer = (int) strtol (token, &end, 10);

/*****************************************************
 * Check to guarantee that the input token is clean. *
 * If a invalid char is found then strtol () stops   *
 * and *end contains the faulty char, otherwise it   *
 * will contain the '\0' char provided by strtok (). *
 *****************************************************/

if (*end != '\0') return FALSE;

/********************************************************
 *  Check to guarantee that the converted integer is    *
 * inside the range of values accepted by the compiler. *
 ********************************************************/

if (errno == ERANGE) return FALSE;

/*************************************************
 * Check to guarantee that the converted integer *
 *  is not smaller than min or larger than max.  *
 *  To disable these tests, make min = INT_MIN   *
 *  or max = INT_MAX when calling this function. *
 *************************************************/
   
if (*integer < min) return FALSE;
if (*integer > max) return FALSE;

return TRUE;
}

/*********** external function **********
 *                                      *
 *   GAMGI_IO_TOKEN_DOUBLE_GET_CHECK    *
 *                                      *
 * Convert a token to double, checking: *
 * 1) if it contains wrong char;        *
 * 2) if it is out of range;            *
 * 3) if it is smaller than min;        *
 * 4) if it is larger than max;         *
 ****************************************/

gamgi_bool gamgi_io_token_double_get_check (const char *token, 
double *real, const double min, const double max)
{
char *end;

/*****************************************************
 * Reset errno, the error variable. This is required *
 * because some C functions set errno when things go *
 * wrong but don't set errno when things go right.   *
 *****************************************************/

errno = 0;

*real = strtod (token, &end);

/*****************************************************
 * Check to guarantee that the input token is clean. *
 * If a invalid char is found then strtod () stops   *
 * and *end contains the faulty char, otherwise it   *
 * will contain the '\0' char provided by strtok (). *
 *****************************************************/

if (*end != '\0') return FALSE;

/********************************************************
 *   Check to guarantee that the converted real is      *
 * inside the range of values accepted by the compiler. *
 ********************************************************/

if (errno == ERANGE) return FALSE;

/*****************************************
 * Check to guarantee that the converted *
 * real is not a NAN (Not A Number)      *
 *****************************************/

if (*real != *real) return FALSE;

/*************************************************
 * Check to guarantee that the converted real    *
 *  is not smaller than min or larger than max.  *
 *  To disable these tests, make min = -DBL_MAX  *
 *  or max = DBL_MAX when calling this function. *
 *************************************************/

if (*real < min) return FALSE;
if (*real > max) return FALSE;

return TRUE;
}

/*********** external function **********
 *                                      *
 *    GAMGI_IO_TOKEN_FLOAT_GET_CHECK    *
 *                                      *
 * Convert a token to float, checking: *
 * 1) if it contains wrong char;        *
 * 2) if it is out of range;            *
 * 3) if it is smaller than min;        *
 * 4) if it is larger than max;         *
 ****************************************/

gamgi_bool gamgi_io_token_float_get_check (const char *token, 
float *real, const float min, const float max)
{
char *end;

/*****************************************************
 * Reset errno, the error variable. This is required *
 * because some C functions set errno when things go *
 * wrong but don't set errno when things go right.   *
 *****************************************************/

errno = 0;

*real = (float) strtod (token, &end);

/*****************************************************
 * Check to guarantee that the input token is clean. *
 * If a invalid char is found then strtod () stops   *
 * and *end contains the faulty char, otherwise it   *
 * will contain the '\0' char provided by strtok (). *
 *****************************************************/

if (*end != '\0') return FALSE;

/********************************************************
 *   Check to guarantee that the converted real is      *
 * inside the range of values accepted by the compiler. *
 ********************************************************/

if (errno == ERANGE) return FALSE;

/*****************************************
 * Check to guarantee that the converted *
 * real is not a NAN (Not A Number)      *
 *****************************************/

if (*real != *real) return FALSE;

/*************************************************
 * Check to guarantee that the converted integer *
 *  is not smaller than min or larger than max.  *
 *  To disable these tests, make min = -FLT_MAX  *
 *  or max = FLT_MAX when calling this function. *
 *************************************************/

if (*real < min) return FALSE;
if (*real > max) return FALSE;

return TRUE;
}

/*********** external function **********
 *                                      *
 *       GAMGI_IO_TOKEN_ALPHA_SCAN      *
 *                                      *
 * Convert a string to alpha, checking: *
 * 1) if it contains less than 1 token; *
 * 2) if it is a good alpha;            *
 * 3) if it contains more than 1 token; *
 ****************************************/

gamgi_bool gamgi_io_token_alpha_scan (const char *string, 
char *alpha, const char *valid, const int length)
{
char *delimiters = GAMGI_IO_DELIMITERS;
char buffer[GAMGI_ENGINE_LINE];
char *token;

if (gamgi_io_token_size_check (string, 
GAMGI_ENGINE_LINE) == FALSE) return FALSE;

/*************************************
 * strtok is a destructive function, *
 * thus use a buffer to save string. *
 *************************************/

strcpy (buffer, string);
token = strtok (buffer, delimiters);
if (token == NULL) return FALSE;

if (gamgi_io_token_alpha_check (token, 
valid, length) == FALSE) return FALSE;

strcpy (alpha, token);

token = strtok (NULL, delimiters);
if (token != NULL) return FALSE;

return TRUE;
}

/************** external function ************
 *                                           *
 *          GAMGI_IO_TOKEN_BOOL_SCAN         *
 *                                           *
 * Convert a string to bool, checking:       *
 * 1) if it contains exactly one good alpha; *
 * 2) if this alpha is "yes" or "no";        *
 *********************************************/

gamgi_bool gamgi_io_token_bool_scan (const char *string, gamgi_bool *bool)
{
char token[GAMGI_ENGINE_TOKEN];

if (gamgi_io_token_alpha_scan (string, token, 
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return FALSE;

if (strcmp (token, "yes") == 0) *bool = TRUE;
else if (strcmp (token, "no") == 0) *bool = FALSE;
else return FALSE;

return TRUE;
}

/************* external function ********
 *                                      *
 *        GAMGI_IO_TOKEN_LONG_SCAN      *
 *                                      *
 * Convert a string to long, checking:  *
 * 1) if it contains less than 1 token; *
 * 2) if it is a good long;             *
 * 3) if it contains more than 1 token; *
 ****************************************/

gamgi_bool gamgi_io_token_long_scan (const char *string, 
long *integer, const long min, const long max)
{
char *delimiters = GAMGI_IO_DELIMITERS;
char *token, buffer[GAMGI_ENGINE_LINE];

if (gamgi_io_token_size_check (string, GAMGI_ENGINE_LINE) == FALSE) return FALSE;

/*************************************
 * strtok is a destructive function, *
 * thus use a buffer to save string. *
 *************************************/

strcpy (buffer, string);
token = strtok (buffer, delimiters);
if (token == NULL) return FALSE;

if (gamgi_io_token_long_get_check (token, integer, min, max) == FALSE) return FALSE;

token = strtok (NULL, delimiters);
if (token != NULL) return FALSE;

return TRUE;
}

/************* external function ********
 *                                      *
 *       GAMGI_IO_TOKEN_INT_SCAN        *
 *                                      *
 * Convert a string to int, checking:   *
 * 1) if it contains less than 1 token; *
 * 2) if it is a good long;             *
 * 3) if it contains more than 1 token; *
 ****************************************/

gamgi_bool gamgi_io_token_int_scan (const char *string, 
int *integer, const int min, const int max)
{
char *delimiters = GAMGI_IO_DELIMITERS;
char *token, buffer[GAMGI_ENGINE_LINE];

if (gamgi_io_token_size_check (string, GAMGI_ENGINE_LINE) == FALSE) return FALSE;

/*************************************
 * strtok is a destructive function, *
 * thus use a buffer to save string. *
 *************************************/

strcpy (buffer, string);
token = strtok (buffer, delimiters);
if (token == NULL) return FALSE;

if (gamgi_io_token_int_get_check (token, integer, min, max) == FALSE) return FALSE;

token = strtok (NULL, delimiters);
if (token != NULL) return FALSE;

return TRUE;
}

/************ external function **********
 *                                       *
 *       GAMGI_IO_TOKEN_DOUBLE_SCAN      *
 *                                       *
 * Convert a string to double, checking: *
 * 1) if it contains less than 1 token;  *
 * 2) if it is a good double;            *
 * 3) if it contains more than 1 token;  *
 *****************************************/

gamgi_bool gamgi_io_token_double_scan (const char *string, 
double *real, const double min, const double max)
{
char *delimiters = GAMGI_IO_DELIMITERS;
char *token, buffer[GAMGI_ENGINE_LINE];

if (gamgi_io_token_size_check (string, GAMGI_ENGINE_LINE) == FALSE) return FALSE;

/*************************************
 * strtok is a destructive function, *
 * thus use a buffer to save string. *
 *************************************/

strcpy (buffer, string);
token = strtok (buffer, delimiters);
if (token == NULL) return FALSE;

if (gamgi_io_token_double_get_check (token, real, min, max) == FALSE) return FALSE;

token = strtok (NULL, delimiters);
if (token != NULL) return FALSE;

return TRUE;
}

/************ external function **********
 *                                       *
 *       GAMGI_IO_TOKEN_FLOAT_SCAN       *
 *                                       *
 * Convert a string to float, checking:  *
 * 1) if it contains less than 1 token;  *
 * 2) if it is a good double;            *
 * 3) if it contains more than 1 token;  *
 *****************************************/

gamgi_bool gamgi_io_token_float_scan (const char *string, 
float *real, const float min, const float max)
{
char *delimiters = GAMGI_IO_DELIMITERS;
char *token, buffer[GAMGI_ENGINE_LINE];

if (gamgi_io_token_size_check (string, GAMGI_ENGINE_LINE) == FALSE) return FALSE;

/*************************************
 * strtok is a destructive function, *
 * thus use a buffer to save string. *
 *************************************/

strcpy (buffer, string);
token = strtok (buffer, delimiters);
if (token == NULL) return FALSE;

if (gamgi_io_token_float_get_check (token, real, min, max) == FALSE) return FALSE;

token = strtok (NULL, delimiters);
if (token != NULL) return FALSE;

return TRUE;
}

/*************** external function ***************
 *                                               *
 *         GAMGI_IO_TOKEN_ALPHA_LONG_SCAN        *
 *                                               *
 * Convert a string to alpha and long, checking: *
 * 1) if it contains less than 1 token;          *
 * 2) if it is a good alpha;                     *
 * 3) if it contains only than 1 token;          *
 * 4) if it is a good long;                      *
 * 5) if it contains more than 2 token;          *
 *************************************************/

gamgi_bool gamgi_io_token_alpha_long_scan (const char *string, 
char *alpha, long *integer, const int length, 
const long min, const long max)
{
char *delimiters = GAMGI_IO_DELIMITERS;
char *token, buffer[GAMGI_ENGINE_LINE];

if (gamgi_io_token_size_check (string, GAMGI_ENGINE_LINE) == FALSE) return FALSE;

/*************************************
 * strtok is a destructive function, *
 * thus use a buffer to save string. *
 *************************************/

strcpy (buffer, string);
token = strtok (buffer, delimiters);
if (token == NULL) return FALSE;

if (gamgi_io_token_alpha_check (token, GAMGI_IO_TEXT, length) == FALSE) return FALSE;

strcpy (alpha, token);

token = strtok (NULL, delimiters);
if (token == NULL) return FALSE;

if (gamgi_io_token_long_get_check (token, integer, min, max) == FALSE) return FALSE;

token = strtok (NULL, delimiters);
if (token != NULL) return FALSE;

return TRUE;
}

void gamgi_io_token_remove (char **new)
{
free (*new);
*new = NULL;
}

void gamgi_io_token_create (const char *old, char **new)
{
int length;

/****************************************
 * copying NULL to NULL is not an error *
 ****************************************/

if (old == NULL) *new = NULL;
else
  {
  /**********************************
   * create and start the new token *
   **********************************/

  length = strlen (old);
  *new = (char *) malloc ((length + 1) * sizeof (char));
  strcpy (*new, old);
  }
}

gamgi_bool gamgi_io_token_check_create (const char *old, char **new, const int max)
{
int length;

/****************************************
 * copying NULL to NULL is not an error *
 ****************************************/

if (old == NULL)
  {
  *new = NULL;
  return TRUE;
  }

/***********************************************
 * Check whether token is larger than max char *
 ***********************************************/

if (gamgi_io_token_size_check (old, max) == FALSE)
  {
  *new = NULL;
  return FALSE;
  }

/**********************************
 * create and start the new token *
 **********************************/

length = strlen (old);
*new = (char *) malloc ((length + 1) * sizeof (char));
strcpy (*new, old);

return TRUE;
}
