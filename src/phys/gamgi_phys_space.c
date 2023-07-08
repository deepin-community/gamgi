/******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_space.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_phys.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_phys_symmetry.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_cell.h"

static void static_range (const int *driver, int site, int *start, int *end)
{
if (site == 1) *start = 0;
else *start = driver[site - 1] + 1;
*end = driver[site];
}

static double static_parser (char *token, double *input)
{
double value[3], sign[2];
int length, n, c;
gamgi_bool number;

/**********************************************************
 * This algorithm explores the fact that all wyckoff      *
 * coordinates have the simplified form: a + b / c        *
 *                                                        *
 * the test for y,z is redundant: 2y or 2z do not exist   *
 *                                                        *
 * the test for + is redundant: + is never the first char *
 **********************************************************/

sign[0] = 1.0;
sign[1] = 1.0;

value[0] = 0.0;
value[1] = 0.0;
value[2] = 1.0;

n = 0;
length = strlen (token);
for (c = 0; c < length; c++)
  {
  
  if (c > 0 && token[c - 1] >= '1' && token[c - 1] <= '9') number = TRUE;
  else number = FALSE;

  switch (token[c])
    {
    case 'x':
    if (number == FALSE) value[n] = input[0];
    else value[n] *= input[0];
    break;

    case 'y':
    if (number == FALSE) value[n] = input[1];
    else value[n] *= input[1];
    break;

    case 'z':
    if (number == FALSE) value[n] = input[2];
    else value[n] *= input[2];
    break;

    case '+':
    if (c > 0) n = 1;
    break;

    case '-':
    if (c > 0) n = 1;
    sign[n] = -1.0;
    break;

    case '/':
    if (n == 0) { value[1] = value[0]; sign[1] = sign[0]; value[0] = 0.0; }
    n = 2;
    break;

    default:
    if (number == FALSE) value[n] = 0.0;
    else value[n] *= 10.0;
    value[n] += token[c] - '0';
    break;
    }
  }

return sign[0] * value[0] + sign[1] * value[1] / value[2];
}

static void static_generate (double *input, const char **wyckoff, 
int start, int n_positions, double *positions)
{
int i;
char token[GAMGI_ENGINE_STRING];
char *token_a, *token_b, *token_c;

for (i = 0; i < n_positions; i++)
  {
  strcpy (token, wyckoff[start + i]);

  /**********************************************
   * parse wyckoff string to get cell positions *
   **********************************************/

  token_a = strtok (token, ",");
  positions[0] = static_parser (token_a, input);
  token_b = strtok (NULL, ",");
  positions[1] = static_parser (token_b, input);
  token_c = strtok (NULL, ",");
  positions[2] = static_parser (token_c, input);

  /****************************************************
   * make sure positions are in the range ]-1.0, 1.0[ *
   ****************************************************/

  gamgi_math_vector_fmod (positions, positions);

  positions += 3;
  }

}

static gamgi_bool static_equal (double *position_i, double *position_j)
{
double r[3], s[3];
double x, y, z;

gamgi_math_cell_range_positive (position_i, r);
gamgi_math_cell_range_positive (position_j, s);

x = fabs (r[0] - s[0]);
y = fabs (r[1] - s[1]);
z = fabs (r[2] - s[2]);

/**********************************************************
 * take translation symmetry into account when comparing  *
 * positions: r,s coordinates are in the range ]0.0, 1.0[ *
 **********************************************************/

if ((x < GAMGI_PHYS_WYCKOFF_TOLERANCE || x > 1.0 - GAMGI_PHYS_WYCKOFF_TOLERANCE)
&& (y < GAMGI_PHYS_WYCKOFF_TOLERANCE || y > 1.0 - GAMGI_PHYS_WYCKOFF_TOLERANCE)
&& (z < GAMGI_PHYS_WYCKOFF_TOLERANCE || z > 1.0 - GAMGI_PHYS_WYCKOFF_TOLERANCE))
  return TRUE;

return FALSE;
}

static void static_compare_one (int n_positions, 
double *positions, gamgi_bool *equal)
{
int i, j;

/*******************************************
 * by default, all positions are different *
 *******************************************/

for (i = 0; i < n_positions; i++) equal[i] = FALSE;

/*******************************
 * find equal positions in the *
 * site selected by the user   *
 *******************************/

for (i = 1; i < n_positions; i++)
  {
  for (j = 0; j < i; j++)
    {
    if (static_equal (positions + 3*i, positions + 3*j) == TRUE)
      equal[i] = TRUE;
    }
  }
}

static void static_compare_two (int n_positions, 
double *positions, double *positions_new, gamgi_bool *equal)
{
int i, j;

/*******************************************
 * by default, all positions are different *
 *******************************************/

for (i = 0; i < n_positions; i++) equal[i] = FALSE;

/*****************************************
 * find equal positions between the site *
 * selected by the user and the new site *
 *****************************************/

for (i = 0; i < n_positions; i++)
  {
  for (j = 0; j < n_positions; j++)
    {
    if (static_equal (positions + 3*i, positions_new + 3*j) == TRUE)
      equal[i] = TRUE;
    }
  }
}

static int static_count (int n_positions, double *positions, gamgi_bool *equal)
{
int i, n_equal = 0;

for (i = 0; i < n_positions; i++)
  {
  if (equal[i] == TRUE) n_equal++;
  else memmove (positions - 3 * n_equal, positions, 3 * sizeof (double));

  positions += 3;
  }

return n_equal;
}

static int static_site (double *input, const char **wyckoff,
const int *driver, int n_positions, double *positions, gamgi_bool *equal)
{
double *positions_new;
int n_positions_new;
int start, end;
int site, n_equal;

positions_new = (double *) malloc (3 * n_positions * sizeof (double));

for (site = 1; site <= driver[0]; site++)
  {
  static_range (driver, site, &start, &end);
  n_positions_new = end - start + 1;

  /*******************************************
   * Wyckoff multiplicity always goes from   *
   * higher to lower values, for all groups, *
   * so it is not necessary to test further  *
   * when the site multiplicity is already   *
   * lower than for the submitted position.  *
   *******************************************/
     
  if (n_positions_new > n_positions) continue;
  if (n_positions_new < n_positions) break;

  static_generate (input, wyckoff, start, n_positions, positions_new);
  static_compare_two (n_positions, positions, positions_new, equal);
  n_equal = static_count (n_positions, positions_new, equal);
  if (n_equal == n_positions) { free (positions_new); return site; }
  }

free (positions_new);
return 0;
}

void gamgi_phys_space_get (int group, const char ***positions, 
const char ***symmetry, const int **driver)
{
switch (group)
  {
  case 1:
  *positions = gamgi_phys_symmetry_wyckoff_1;
  *symmetry = gamgi_phys_symmetry_positions_1;
  *driver = gamgi_phys_symmetry_driver_1;
  break;

  case 2:
  *positions = gamgi_phys_symmetry_wyckoff_2;
  *symmetry = gamgi_phys_symmetry_positions_2;
  *driver = gamgi_phys_symmetry_driver_2;
  break;

  case 3:
  *positions = gamgi_phys_symmetry_wyckoff_3;
  *symmetry = gamgi_phys_symmetry_positions_3;
  *driver = gamgi_phys_symmetry_driver_3;
  break;

  case 4:
  *positions = gamgi_phys_symmetry_wyckoff_4;
  *symmetry = gamgi_phys_symmetry_positions_4;
  *driver = gamgi_phys_symmetry_driver_4;
  break;

  case 5:
  *positions = gamgi_phys_symmetry_wyckoff_5;
  *symmetry = gamgi_phys_symmetry_positions_5;
  *driver = gamgi_phys_symmetry_driver_5;
  break;

  case 6:
  *positions = gamgi_phys_symmetry_wyckoff_6;
  *symmetry = gamgi_phys_symmetry_positions_6;
  *driver = gamgi_phys_symmetry_driver_6;
  break;

  case 7:
  *positions = gamgi_phys_symmetry_wyckoff_7;
  *symmetry = gamgi_phys_symmetry_positions_7;
  *driver = gamgi_phys_symmetry_driver_7;
  break;

  case 8:
  *positions = gamgi_phys_symmetry_wyckoff_8;
  *symmetry = gamgi_phys_symmetry_positions_8;
  *driver = gamgi_phys_symmetry_driver_8;
  break;

  case 9:
  *positions = gamgi_phys_symmetry_wyckoff_9;
  *symmetry = gamgi_phys_symmetry_positions_9;
  *driver = gamgi_phys_symmetry_driver_9;
  break;

  case 10:
  *positions = gamgi_phys_symmetry_wyckoff_10;
  *symmetry = gamgi_phys_symmetry_positions_10;
  *driver = gamgi_phys_symmetry_driver_10;
  break;

  case 11:
  *positions = gamgi_phys_symmetry_wyckoff_11;
  *symmetry = gamgi_phys_symmetry_positions_11;
  *driver = gamgi_phys_symmetry_driver_11;
  break;

  case 12:
  *positions = gamgi_phys_symmetry_wyckoff_12;
  *symmetry = gamgi_phys_symmetry_positions_12;
  *driver = gamgi_phys_symmetry_driver_12;
  break;

  case 13:
  *positions = gamgi_phys_symmetry_wyckoff_13;
  *symmetry = gamgi_phys_symmetry_positions_13;
  *driver = gamgi_phys_symmetry_driver_13;
  break;

  case 14:
  *positions = gamgi_phys_symmetry_wyckoff_14;
  *symmetry = gamgi_phys_symmetry_positions_14;
  *driver = gamgi_phys_symmetry_driver_14;
  break;

  case 15:
  *positions = gamgi_phys_symmetry_wyckoff_15;
  *symmetry = gamgi_phys_symmetry_positions_15;
  *driver = gamgi_phys_symmetry_driver_15;
  break;

  case 16:
  *positions = gamgi_phys_symmetry_wyckoff_16;
  *symmetry = gamgi_phys_symmetry_positions_16;
  *driver = gamgi_phys_symmetry_driver_16;
  break;

  case 17:
  *positions = gamgi_phys_symmetry_wyckoff_17;
  *symmetry = gamgi_phys_symmetry_positions_17;
  *driver = gamgi_phys_symmetry_driver_17;
  break;

  case 18:
  *positions = gamgi_phys_symmetry_wyckoff_18;
  *symmetry = gamgi_phys_symmetry_positions_18;
  *driver = gamgi_phys_symmetry_driver_18;
  break;

  case 19:
  *positions = gamgi_phys_symmetry_wyckoff_19;
  *symmetry = gamgi_phys_symmetry_positions_19;
  *driver = gamgi_phys_symmetry_driver_19;
  break;

  case 20:
  *positions = gamgi_phys_symmetry_wyckoff_20;
  *symmetry = gamgi_phys_symmetry_positions_20;
  *driver = gamgi_phys_symmetry_driver_20;
  break;

  case 21:
  *positions = gamgi_phys_symmetry_wyckoff_21;
  *symmetry = gamgi_phys_symmetry_positions_21;
  *driver = gamgi_phys_symmetry_driver_21;
  break;

  case 22:
  *positions = gamgi_phys_symmetry_wyckoff_22;
  *symmetry = gamgi_phys_symmetry_positions_22;
  *driver = gamgi_phys_symmetry_driver_22;
  break;

  case 23:
  *positions = gamgi_phys_symmetry_wyckoff_23;
  *symmetry = gamgi_phys_symmetry_positions_23;
  *driver = gamgi_phys_symmetry_driver_23;
  break;

  case 24:
  *positions = gamgi_phys_symmetry_wyckoff_24;
  *symmetry = gamgi_phys_symmetry_positions_24;
  *driver = gamgi_phys_symmetry_driver_24;
  break;

  case 25:
  *positions = gamgi_phys_symmetry_wyckoff_25;
  *symmetry = gamgi_phys_symmetry_positions_25;
  *driver = gamgi_phys_symmetry_driver_25;
  break;

  case 26:
  *positions = gamgi_phys_symmetry_wyckoff_26;
  *symmetry = gamgi_phys_symmetry_positions_26;
  *driver = gamgi_phys_symmetry_driver_26;
  break;

  case 27:
  *positions = gamgi_phys_symmetry_wyckoff_27;
  *symmetry = gamgi_phys_symmetry_positions_27;
  *driver = gamgi_phys_symmetry_driver_27;
  break;

  case 28:
  *positions = gamgi_phys_symmetry_wyckoff_28;
  *symmetry = gamgi_phys_symmetry_positions_28;
  *driver = gamgi_phys_symmetry_driver_28;
  break;

  case 29:
  *positions = gamgi_phys_symmetry_wyckoff_29;
  *symmetry = gamgi_phys_symmetry_positions_29;
  *driver = gamgi_phys_symmetry_driver_29;
  break;

  case 30:
  *positions = gamgi_phys_symmetry_wyckoff_30;
  *symmetry = gamgi_phys_symmetry_positions_30;
  *driver = gamgi_phys_symmetry_driver_30;
  break;

  case 31:
  *positions = gamgi_phys_symmetry_wyckoff_31;
  *symmetry = gamgi_phys_symmetry_positions_31;
  *driver = gamgi_phys_symmetry_driver_31;
  break;

  case 32:
  *positions = gamgi_phys_symmetry_wyckoff_32;
  *symmetry = gamgi_phys_symmetry_positions_32;
  *driver = gamgi_phys_symmetry_driver_32;
  break;

  case 33:
  *positions = gamgi_phys_symmetry_wyckoff_33;
  *symmetry = gamgi_phys_symmetry_positions_33;
  *driver = gamgi_phys_symmetry_driver_33;
  break;

  case 34:
  *positions = gamgi_phys_symmetry_wyckoff_34;
  *symmetry = gamgi_phys_symmetry_positions_34;
  *driver = gamgi_phys_symmetry_driver_34;
  break;

  case 35:
  *positions = gamgi_phys_symmetry_wyckoff_35;
  *symmetry = gamgi_phys_symmetry_positions_35;
  *driver = gamgi_phys_symmetry_driver_35;
  break;

  case 36:
  *positions = gamgi_phys_symmetry_wyckoff_36;
  *symmetry = gamgi_phys_symmetry_positions_36;
  *driver = gamgi_phys_symmetry_driver_36;
  break;

  case 37:
  *positions = gamgi_phys_symmetry_wyckoff_37;
  *symmetry = gamgi_phys_symmetry_positions_37;
  *driver = gamgi_phys_symmetry_driver_37;
  break;

  case 38:
  *positions = gamgi_phys_symmetry_wyckoff_38;
  *symmetry = gamgi_phys_symmetry_positions_38;
  *driver = gamgi_phys_symmetry_driver_38;
  break;

  case 39:
  *positions = gamgi_phys_symmetry_wyckoff_39;
  *symmetry = gamgi_phys_symmetry_positions_39;
  *driver = gamgi_phys_symmetry_driver_39;
  break;

  case 40:
  *positions = gamgi_phys_symmetry_wyckoff_40;
  *symmetry = gamgi_phys_symmetry_positions_40;
  *driver = gamgi_phys_symmetry_driver_40;
  break;

  case 41:
  *positions = gamgi_phys_symmetry_wyckoff_41;
  *symmetry = gamgi_phys_symmetry_positions_41;
  *driver = gamgi_phys_symmetry_driver_41;
  break;

  case 42:
  *positions = gamgi_phys_symmetry_wyckoff_42;
  *symmetry = gamgi_phys_symmetry_positions_42;
  *driver = gamgi_phys_symmetry_driver_42;
  break;

  case 43:
  *positions = gamgi_phys_symmetry_wyckoff_43;
  *symmetry = gamgi_phys_symmetry_positions_43;
  *driver = gamgi_phys_symmetry_driver_43;
  break;

  case 44:
  *positions = gamgi_phys_symmetry_wyckoff_44;
  *symmetry = gamgi_phys_symmetry_positions_44;
  *driver = gamgi_phys_symmetry_driver_44;
  break;

  case 45:
  *positions = gamgi_phys_symmetry_wyckoff_45;
  *symmetry = gamgi_phys_symmetry_positions_45;
  *driver = gamgi_phys_symmetry_driver_45;
  break;

  case 46:
  *positions = gamgi_phys_symmetry_wyckoff_46;
  *symmetry = gamgi_phys_symmetry_positions_46;
  *driver = gamgi_phys_symmetry_driver_46;
  break;

  case 47:
  *positions = gamgi_phys_symmetry_wyckoff_47;
  *symmetry = gamgi_phys_symmetry_positions_47;
  *driver = gamgi_phys_symmetry_driver_47;
  break;

  case 48:
  *positions = gamgi_phys_symmetry_wyckoff_48;
  *symmetry = gamgi_phys_symmetry_positions_48;
  *driver = gamgi_phys_symmetry_driver_48;
  break;

  case 49:
  *positions = gamgi_phys_symmetry_wyckoff_49;
  *symmetry = gamgi_phys_symmetry_positions_49;
  *driver = gamgi_phys_symmetry_driver_49;
  break;

  case 50:
  *positions = gamgi_phys_symmetry_wyckoff_50;
  *symmetry = gamgi_phys_symmetry_positions_50;
  *driver = gamgi_phys_symmetry_driver_50;
  break;

  case 51:
  *positions = gamgi_phys_symmetry_wyckoff_51;
  *symmetry = gamgi_phys_symmetry_positions_51;
  *driver = gamgi_phys_symmetry_driver_51;
  break;

  case 52:
  *positions = gamgi_phys_symmetry_wyckoff_52;
  *symmetry = gamgi_phys_symmetry_positions_52;
  *driver = gamgi_phys_symmetry_driver_52;
  break;

  case 53:
  *positions = gamgi_phys_symmetry_wyckoff_53;
  *symmetry = gamgi_phys_symmetry_positions_53;
  *driver = gamgi_phys_symmetry_driver_53;
  break;

  case 54:
  *positions = gamgi_phys_symmetry_wyckoff_54;
  *symmetry = gamgi_phys_symmetry_positions_54;
  *driver = gamgi_phys_symmetry_driver_54;
  break;

  case 55:
  *positions = gamgi_phys_symmetry_wyckoff_55;
  *symmetry = gamgi_phys_symmetry_positions_55;
  *driver = gamgi_phys_symmetry_driver_55;
  break;

  case 56:
  *positions = gamgi_phys_symmetry_wyckoff_56;
  *symmetry = gamgi_phys_symmetry_positions_56;
  *driver = gamgi_phys_symmetry_driver_56;
  break;

  case 57:
  *positions = gamgi_phys_symmetry_wyckoff_57;
  *symmetry = gamgi_phys_symmetry_positions_57;
  *driver = gamgi_phys_symmetry_driver_57;
  break;

  case 58:
  *positions = gamgi_phys_symmetry_wyckoff_58;
  *symmetry = gamgi_phys_symmetry_positions_58;
  *driver = gamgi_phys_symmetry_driver_58;
  break;

  case 59:
  *positions = gamgi_phys_symmetry_wyckoff_59;
  *symmetry = gamgi_phys_symmetry_positions_59;
  *driver = gamgi_phys_symmetry_driver_59;
  break;

  case 60:
  *positions = gamgi_phys_symmetry_wyckoff_60;
  *symmetry = gamgi_phys_symmetry_positions_60;
  *driver = gamgi_phys_symmetry_driver_60;
  break;

  case 61:
  *positions = gamgi_phys_symmetry_wyckoff_61;
  *symmetry = gamgi_phys_symmetry_positions_61;
  *driver = gamgi_phys_symmetry_driver_61;
  break;

  case 62:
  *positions = gamgi_phys_symmetry_wyckoff_62;
  *symmetry = gamgi_phys_symmetry_positions_62;
  *driver = gamgi_phys_symmetry_driver_62;
  break;

  case 63:
  *positions = gamgi_phys_symmetry_wyckoff_63;
  *symmetry = gamgi_phys_symmetry_positions_63;
  *driver = gamgi_phys_symmetry_driver_63;
  break;

  case 64:
  *positions = gamgi_phys_symmetry_wyckoff_64;
  *symmetry = gamgi_phys_symmetry_positions_64;
  *driver = gamgi_phys_symmetry_driver_64;
  break;

  case 65:
  *positions = gamgi_phys_symmetry_wyckoff_65;
  *symmetry = gamgi_phys_symmetry_positions_65;
  *driver = gamgi_phys_symmetry_driver_65;
  break;

  case 66:
  *positions = gamgi_phys_symmetry_wyckoff_66;
  *symmetry = gamgi_phys_symmetry_positions_66;
  *driver = gamgi_phys_symmetry_driver_66;
  break;

  case 67:
  *positions = gamgi_phys_symmetry_wyckoff_67;
  *symmetry = gamgi_phys_symmetry_positions_67;
  *driver = gamgi_phys_symmetry_driver_67;
  break;

  case 68:
  *positions = gamgi_phys_symmetry_wyckoff_68;
  *symmetry = gamgi_phys_symmetry_positions_68;
  *driver = gamgi_phys_symmetry_driver_68;
  break;

  case 69:
  *positions = gamgi_phys_symmetry_wyckoff_69;
  *symmetry = gamgi_phys_symmetry_positions_69;
  *driver = gamgi_phys_symmetry_driver_69;
  break;

  case 70:
  *positions = gamgi_phys_symmetry_wyckoff_70;
  *symmetry = gamgi_phys_symmetry_positions_70;
  *driver = gamgi_phys_symmetry_driver_70;
  break;

  case 71:
  *positions = gamgi_phys_symmetry_wyckoff_71;
  *symmetry = gamgi_phys_symmetry_positions_71;
  *driver = gamgi_phys_symmetry_driver_71;
  break;

  case 72:
  *positions = gamgi_phys_symmetry_wyckoff_72;
  *symmetry = gamgi_phys_symmetry_positions_72;
  *driver = gamgi_phys_symmetry_driver_72;
  break;

  case 73:
  *positions = gamgi_phys_symmetry_wyckoff_73;
  *symmetry = gamgi_phys_symmetry_positions_73;
  *driver = gamgi_phys_symmetry_driver_73;
  break;

  case 74:
  *positions = gamgi_phys_symmetry_wyckoff_74;
  *symmetry = gamgi_phys_symmetry_positions_74;
  *driver = gamgi_phys_symmetry_driver_74;
  break;

  case 75:
  *positions = gamgi_phys_symmetry_wyckoff_75;
  *symmetry = gamgi_phys_symmetry_positions_75;
  *driver = gamgi_phys_symmetry_driver_75;
  break;

  case 76:
  *positions = gamgi_phys_symmetry_wyckoff_76;
  *symmetry = gamgi_phys_symmetry_positions_76;
  *driver = gamgi_phys_symmetry_driver_76;
  break;

  case 77:
  *positions = gamgi_phys_symmetry_wyckoff_77;
  *symmetry = gamgi_phys_symmetry_positions_77;
  *driver = gamgi_phys_symmetry_driver_77;
  break;

  case 78:
  *positions = gamgi_phys_symmetry_wyckoff_78;
  *symmetry = gamgi_phys_symmetry_positions_78;
  *driver = gamgi_phys_symmetry_driver_78;
  break;

  case 79:
  *positions = gamgi_phys_symmetry_wyckoff_79;
  *symmetry = gamgi_phys_symmetry_positions_79;
  *driver = gamgi_phys_symmetry_driver_79;
  break;

  case 80:
  *positions = gamgi_phys_symmetry_wyckoff_80;
  *symmetry = gamgi_phys_symmetry_positions_80;
  *driver = gamgi_phys_symmetry_driver_80;
  break;

  case 81:
  *positions = gamgi_phys_symmetry_wyckoff_81;
  *symmetry = gamgi_phys_symmetry_positions_81;
  *driver = gamgi_phys_symmetry_driver_81;
  break;

  case 82:
  *positions = gamgi_phys_symmetry_wyckoff_82;
  *symmetry = gamgi_phys_symmetry_positions_82;
  *driver = gamgi_phys_symmetry_driver_82;
  break;

  case 83:
  *positions = gamgi_phys_symmetry_wyckoff_83;
  *symmetry = gamgi_phys_symmetry_positions_83;
  *driver = gamgi_phys_symmetry_driver_83;
  break;

  case 84:
  *positions = gamgi_phys_symmetry_wyckoff_84;
  *symmetry = gamgi_phys_symmetry_positions_84;
  *driver = gamgi_phys_symmetry_driver_84;
  break;

  case 85:
  *positions = gamgi_phys_symmetry_wyckoff_85;
  *symmetry = gamgi_phys_symmetry_positions_85;
  *driver = gamgi_phys_symmetry_driver_85;
  break;

  case 86:
  *positions = gamgi_phys_symmetry_wyckoff_86;
  *symmetry = gamgi_phys_symmetry_positions_86;
  *driver = gamgi_phys_symmetry_driver_86;
  break;

  case 87:
  *positions = gamgi_phys_symmetry_wyckoff_87;
  *symmetry = gamgi_phys_symmetry_positions_87;
  *driver = gamgi_phys_symmetry_driver_87;
  break;

  case 88:
  *positions = gamgi_phys_symmetry_wyckoff_88;
  *symmetry = gamgi_phys_symmetry_positions_88;
  *driver = gamgi_phys_symmetry_driver_88;
  break;

  case 89:
  *positions = gamgi_phys_symmetry_wyckoff_89;
  *symmetry = gamgi_phys_symmetry_positions_89;
  *driver = gamgi_phys_symmetry_driver_89;
  break;

  case 90:
  *positions = gamgi_phys_symmetry_wyckoff_90;
  *symmetry = gamgi_phys_symmetry_positions_90;
  *driver = gamgi_phys_symmetry_driver_90;
  break;

  case 91:
  *positions = gamgi_phys_symmetry_wyckoff_91;
  *symmetry = gamgi_phys_symmetry_positions_91;
  *driver = gamgi_phys_symmetry_driver_91;
  break;

  case 92:
  *positions = gamgi_phys_symmetry_wyckoff_92;
  *symmetry = gamgi_phys_symmetry_positions_92;
  *driver = gamgi_phys_symmetry_driver_92;
  break;

  case 93:
  *positions = gamgi_phys_symmetry_wyckoff_93;
  *symmetry = gamgi_phys_symmetry_positions_93;
  *driver = gamgi_phys_symmetry_driver_93;
  break;

  case 94:
  *positions = gamgi_phys_symmetry_wyckoff_94;
  *symmetry = gamgi_phys_symmetry_positions_94;
  *driver = gamgi_phys_symmetry_driver_94;
  break;

  case 95:
  *positions = gamgi_phys_symmetry_wyckoff_95;
  *symmetry = gamgi_phys_symmetry_positions_95;
  *driver = gamgi_phys_symmetry_driver_95;
  break;

  case 96:
  *positions = gamgi_phys_symmetry_wyckoff_96;
  *symmetry = gamgi_phys_symmetry_positions_96;
  *driver = gamgi_phys_symmetry_driver_96;
  break;

  case 97:
  *positions = gamgi_phys_symmetry_wyckoff_97;
  *symmetry = gamgi_phys_symmetry_positions_97;
  *driver = gamgi_phys_symmetry_driver_97;
  break;

  case 98:
  *positions = gamgi_phys_symmetry_wyckoff_98;
  *symmetry = gamgi_phys_symmetry_positions_98;
  *driver = gamgi_phys_symmetry_driver_98;
  break;

  case 99:
  *positions = gamgi_phys_symmetry_wyckoff_99;
  *symmetry = gamgi_phys_symmetry_positions_99;
  *driver = gamgi_phys_symmetry_driver_99;
  break;

  case 100:
  *positions = gamgi_phys_symmetry_wyckoff_100;
  *symmetry = gamgi_phys_symmetry_positions_100;
  *driver = gamgi_phys_symmetry_driver_100;
  break;

  case 101:
  *positions = gamgi_phys_symmetry_wyckoff_101;
  *symmetry = gamgi_phys_symmetry_positions_101;
  *driver = gamgi_phys_symmetry_driver_101;
  break;

  case 102:
  *positions = gamgi_phys_symmetry_wyckoff_102;
  *symmetry = gamgi_phys_symmetry_positions_102;
  *driver = gamgi_phys_symmetry_driver_102;
  break;

  case 103:
  *positions = gamgi_phys_symmetry_wyckoff_103;
  *symmetry = gamgi_phys_symmetry_positions_103;
  *driver = gamgi_phys_symmetry_driver_103;
  break;

  case 104:
  *positions = gamgi_phys_symmetry_wyckoff_104;
  *symmetry = gamgi_phys_symmetry_positions_104;
  *driver = gamgi_phys_symmetry_driver_104;
  break;

  case 105:
  *positions = gamgi_phys_symmetry_wyckoff_105;
  *symmetry = gamgi_phys_symmetry_positions_105;
  *driver = gamgi_phys_symmetry_driver_105;
  break;

  case 106:
  *positions = gamgi_phys_symmetry_wyckoff_106;
  *symmetry = gamgi_phys_symmetry_positions_106;
  *driver = gamgi_phys_symmetry_driver_106;
  break;

  case 107:
  *positions = gamgi_phys_symmetry_wyckoff_107;
  *symmetry = gamgi_phys_symmetry_positions_107;
  *driver = gamgi_phys_symmetry_driver_107;
  break;

  case 108:
  *positions = gamgi_phys_symmetry_wyckoff_108;
  *symmetry = gamgi_phys_symmetry_positions_108;
  *driver = gamgi_phys_symmetry_driver_108;
  break;

  case 109:
  *positions = gamgi_phys_symmetry_wyckoff_109;
  *symmetry = gamgi_phys_symmetry_positions_109;
  *driver = gamgi_phys_symmetry_driver_109;
  break;

  case 110:
  *positions = gamgi_phys_symmetry_wyckoff_110;
  *symmetry = gamgi_phys_symmetry_positions_110;
  *driver = gamgi_phys_symmetry_driver_110;
  break;

  case 111:
  *positions = gamgi_phys_symmetry_wyckoff_111;
  *symmetry = gamgi_phys_symmetry_positions_111;
  *driver = gamgi_phys_symmetry_driver_111;
  break;

  case 112:
  *positions = gamgi_phys_symmetry_wyckoff_112;
  *symmetry = gamgi_phys_symmetry_positions_112;
  *driver = gamgi_phys_symmetry_driver_112;
  break;

  case 113:
  *positions = gamgi_phys_symmetry_wyckoff_113;
  *symmetry = gamgi_phys_symmetry_positions_113;
  *driver = gamgi_phys_symmetry_driver_113;
  break;

  case 114:
  *positions = gamgi_phys_symmetry_wyckoff_114;
  *symmetry = gamgi_phys_symmetry_positions_114;
  *driver = gamgi_phys_symmetry_driver_114;
  break;

  case 115:
  *positions = gamgi_phys_symmetry_wyckoff_115;
  *symmetry = gamgi_phys_symmetry_positions_115;
  *driver = gamgi_phys_symmetry_driver_115;
  break;

  case 116:
  *positions = gamgi_phys_symmetry_wyckoff_116;
  *symmetry = gamgi_phys_symmetry_positions_116;
  *driver = gamgi_phys_symmetry_driver_116;
  break;

  case 117:
  *positions = gamgi_phys_symmetry_wyckoff_117;
  *symmetry = gamgi_phys_symmetry_positions_117;
  *driver = gamgi_phys_symmetry_driver_117;
  break;

  case 118:
  *positions = gamgi_phys_symmetry_wyckoff_118;
  *symmetry = gamgi_phys_symmetry_positions_118;
  *driver = gamgi_phys_symmetry_driver_118;
  break;

  case 119:
  *positions = gamgi_phys_symmetry_wyckoff_119;
  *symmetry = gamgi_phys_symmetry_positions_119;
  *driver = gamgi_phys_symmetry_driver_119;
  break;

  case 120:
  *positions = gamgi_phys_symmetry_wyckoff_120;
  *symmetry = gamgi_phys_symmetry_positions_120;
  *driver = gamgi_phys_symmetry_driver_120;
  break;

  case 121:
  *positions = gamgi_phys_symmetry_wyckoff_121;
  *symmetry = gamgi_phys_symmetry_positions_121;
  *driver = gamgi_phys_symmetry_driver_121;
  break;

  case 122:
  *positions = gamgi_phys_symmetry_wyckoff_122;
  *symmetry = gamgi_phys_symmetry_positions_122;
  *driver = gamgi_phys_symmetry_driver_122;
  break;

  case 123:
  *positions = gamgi_phys_symmetry_wyckoff_123;
  *symmetry = gamgi_phys_symmetry_positions_123;
  *driver = gamgi_phys_symmetry_driver_123;
  break;

  case 124:
  *positions = gamgi_phys_symmetry_wyckoff_124;
  *symmetry = gamgi_phys_symmetry_positions_124;
  *driver = gamgi_phys_symmetry_driver_124;
  break;

  case 125:
  *positions = gamgi_phys_symmetry_wyckoff_125;
  *symmetry = gamgi_phys_symmetry_positions_125;
  *driver = gamgi_phys_symmetry_driver_125;
  break;

  case 126:
  *positions = gamgi_phys_symmetry_wyckoff_126;
  *symmetry = gamgi_phys_symmetry_positions_126;
  *driver = gamgi_phys_symmetry_driver_126;
  break;

  case 127:
  *positions = gamgi_phys_symmetry_wyckoff_127;
  *symmetry = gamgi_phys_symmetry_positions_127;
  *driver = gamgi_phys_symmetry_driver_127;
  break;

  case 128:
  *positions = gamgi_phys_symmetry_wyckoff_128;
  *symmetry = gamgi_phys_symmetry_positions_128;
  *driver = gamgi_phys_symmetry_driver_128;
  break;

  case 129:
  *positions = gamgi_phys_symmetry_wyckoff_129;
  *symmetry = gamgi_phys_symmetry_positions_129;
  *driver = gamgi_phys_symmetry_driver_129;
  break;

  case 130:
  *positions = gamgi_phys_symmetry_wyckoff_130;
  *symmetry = gamgi_phys_symmetry_positions_130;
  *driver = gamgi_phys_symmetry_driver_130;
  break;

  case 131:
  *positions = gamgi_phys_symmetry_wyckoff_131;
  *symmetry = gamgi_phys_symmetry_positions_131;
  *driver = gamgi_phys_symmetry_driver_131;
  break;

  case 132:
  *positions = gamgi_phys_symmetry_wyckoff_132;
  *symmetry = gamgi_phys_symmetry_positions_132;
  *driver = gamgi_phys_symmetry_driver_132;
  break;

  case 133:
  *positions = gamgi_phys_symmetry_wyckoff_133;
  *symmetry = gamgi_phys_symmetry_positions_133;
  *driver = gamgi_phys_symmetry_driver_133;
  break;

  case 134:
  *positions = gamgi_phys_symmetry_wyckoff_134;
  *symmetry = gamgi_phys_symmetry_positions_134;
  *driver = gamgi_phys_symmetry_driver_134;
  break;

  case 135:
  *positions = gamgi_phys_symmetry_wyckoff_135;
  *symmetry = gamgi_phys_symmetry_positions_135;
  *driver = gamgi_phys_symmetry_driver_135;
  break;

  case 136:
  *positions = gamgi_phys_symmetry_wyckoff_136;
  *symmetry = gamgi_phys_symmetry_positions_136;
  *driver = gamgi_phys_symmetry_driver_136;
  break;

  case 137:
  *positions = gamgi_phys_symmetry_wyckoff_137;
  *symmetry = gamgi_phys_symmetry_positions_137;
  *driver = gamgi_phys_symmetry_driver_137;
  break;

  case 138:
  *positions = gamgi_phys_symmetry_wyckoff_138;
  *symmetry = gamgi_phys_symmetry_positions_138;
  *driver = gamgi_phys_symmetry_driver_138;
  break;

  case 139:
  *positions = gamgi_phys_symmetry_wyckoff_139;
  *symmetry = gamgi_phys_symmetry_positions_139;
  *driver = gamgi_phys_symmetry_driver_139;
  break;

  case 140:
  *positions = gamgi_phys_symmetry_wyckoff_140;
  *symmetry = gamgi_phys_symmetry_positions_140;
  *driver = gamgi_phys_symmetry_driver_140;
  break;

  case 141:
  *positions = gamgi_phys_symmetry_wyckoff_141;
  *symmetry = gamgi_phys_symmetry_positions_141;
  *driver = gamgi_phys_symmetry_driver_141;
  break;

  case 142:
  *positions = gamgi_phys_symmetry_wyckoff_142;
  *symmetry = gamgi_phys_symmetry_positions_142;
  *driver = gamgi_phys_symmetry_driver_142;
  break;

  case 143:
  *positions = gamgi_phys_symmetry_wyckoff_143;
  *symmetry = gamgi_phys_symmetry_positions_143;
  *driver = gamgi_phys_symmetry_driver_143;
  break;

  case 144:
  *positions = gamgi_phys_symmetry_wyckoff_144;
  *symmetry = gamgi_phys_symmetry_positions_144;
  *driver = gamgi_phys_symmetry_driver_144;
  break;

  case 145:
  *positions = gamgi_phys_symmetry_wyckoff_145;
  *symmetry = gamgi_phys_symmetry_positions_145;
  *driver = gamgi_phys_symmetry_driver_145;
  break;

  case 146:
  *positions = gamgi_phys_symmetry_wyckoff_146;
  *symmetry = gamgi_phys_symmetry_positions_146;
  *driver = gamgi_phys_symmetry_driver_146;
  break;

  case 147:
  *positions = gamgi_phys_symmetry_wyckoff_147;
  *symmetry = gamgi_phys_symmetry_positions_147;
  *driver = gamgi_phys_symmetry_driver_147;
  break;

  case 148:
  *positions = gamgi_phys_symmetry_wyckoff_148;
  *symmetry = gamgi_phys_symmetry_positions_148;
  *driver = gamgi_phys_symmetry_driver_148;
  break;

  case 149:
  *positions = gamgi_phys_symmetry_wyckoff_149;
  *symmetry = gamgi_phys_symmetry_positions_149;
  *driver = gamgi_phys_symmetry_driver_149;
  break;

  case 150:
  *positions = gamgi_phys_symmetry_wyckoff_150;
  *symmetry = gamgi_phys_symmetry_positions_150;
  *driver = gamgi_phys_symmetry_driver_150;
  break;

  case 151:
  *positions = gamgi_phys_symmetry_wyckoff_151;
  *symmetry = gamgi_phys_symmetry_positions_151;
  *driver = gamgi_phys_symmetry_driver_151;
  break;

  case 152:
  *positions = gamgi_phys_symmetry_wyckoff_152;
  *symmetry = gamgi_phys_symmetry_positions_152;
  *driver = gamgi_phys_symmetry_driver_152;
  break;

  case 153:
  *positions = gamgi_phys_symmetry_wyckoff_153;
  *symmetry = gamgi_phys_symmetry_positions_153;
  *driver = gamgi_phys_symmetry_driver_153;
  break;

  case 154:
  *positions = gamgi_phys_symmetry_wyckoff_154;
  *symmetry = gamgi_phys_symmetry_positions_154;
  *driver = gamgi_phys_symmetry_driver_154;
  break;

  case 155:
  *positions = gamgi_phys_symmetry_wyckoff_155;
  *symmetry = gamgi_phys_symmetry_positions_155;
  *driver = gamgi_phys_symmetry_driver_155;
  break;

  case 156:
  *positions = gamgi_phys_symmetry_wyckoff_156;
  *symmetry = gamgi_phys_symmetry_positions_156;
  *driver = gamgi_phys_symmetry_driver_156;
  break;

  case 157:
  *positions = gamgi_phys_symmetry_wyckoff_157;
  *symmetry = gamgi_phys_symmetry_positions_157;
  *driver = gamgi_phys_symmetry_driver_157;
  break;

  case 158:
  *positions = gamgi_phys_symmetry_wyckoff_158;
  *symmetry = gamgi_phys_symmetry_positions_158;
  *driver = gamgi_phys_symmetry_driver_158;
  break;

  case 159:
  *positions = gamgi_phys_symmetry_wyckoff_159;
  *symmetry = gamgi_phys_symmetry_positions_159;
  *driver = gamgi_phys_symmetry_driver_159;
  break;

  case 160:
  *positions = gamgi_phys_symmetry_wyckoff_160;
  *symmetry = gamgi_phys_symmetry_positions_160;
  *driver = gamgi_phys_symmetry_driver_160;
  break;

  case 161:
  *positions = gamgi_phys_symmetry_wyckoff_161;
  *symmetry = gamgi_phys_symmetry_positions_161;
  *driver = gamgi_phys_symmetry_driver_161;
  break;

  case 162:
  *positions = gamgi_phys_symmetry_wyckoff_162;
  *symmetry = gamgi_phys_symmetry_positions_162;
  *driver = gamgi_phys_symmetry_driver_162;
  break;

  case 163:
  *positions = gamgi_phys_symmetry_wyckoff_163;
  *symmetry = gamgi_phys_symmetry_positions_163;
  *driver = gamgi_phys_symmetry_driver_163;
  break;

  case 164:
  *positions = gamgi_phys_symmetry_wyckoff_164;
  *symmetry = gamgi_phys_symmetry_positions_164;
  *driver = gamgi_phys_symmetry_driver_164;
  break;

  case 165:
  *positions = gamgi_phys_symmetry_wyckoff_165;
  *symmetry = gamgi_phys_symmetry_positions_165;
  *driver = gamgi_phys_symmetry_driver_165;
  break;

  case 166:
  *positions = gamgi_phys_symmetry_wyckoff_166;
  *symmetry = gamgi_phys_symmetry_positions_166;
  *driver = gamgi_phys_symmetry_driver_166;
  break;

  case 167:
  *positions = gamgi_phys_symmetry_wyckoff_167;
  *symmetry = gamgi_phys_symmetry_positions_167;
  *driver = gamgi_phys_symmetry_driver_167;
  break;

  case 168:
  *positions = gamgi_phys_symmetry_wyckoff_168;
  *symmetry = gamgi_phys_symmetry_positions_168;
  *driver = gamgi_phys_symmetry_driver_168;
  break;

  case 169:
  *positions = gamgi_phys_symmetry_wyckoff_169;
  *symmetry = gamgi_phys_symmetry_positions_169;
  *driver = gamgi_phys_symmetry_driver_169;
  break;

  case 170:
  *positions = gamgi_phys_symmetry_wyckoff_170;
  *symmetry = gamgi_phys_symmetry_positions_170;
  *driver = gamgi_phys_symmetry_driver_170;
  break;

  case 171:
  *positions = gamgi_phys_symmetry_wyckoff_171;
  *symmetry = gamgi_phys_symmetry_positions_171;
  *driver = gamgi_phys_symmetry_driver_171;
  break;

  case 172:
  *positions = gamgi_phys_symmetry_wyckoff_172;
  *symmetry = gamgi_phys_symmetry_positions_172;
  *driver = gamgi_phys_symmetry_driver_172;
  break;

  case 173:
  *positions = gamgi_phys_symmetry_wyckoff_173;
  *symmetry = gamgi_phys_symmetry_positions_173;
  *driver = gamgi_phys_symmetry_driver_173;
  break;

  case 174:
  *positions = gamgi_phys_symmetry_wyckoff_174;
  *symmetry = gamgi_phys_symmetry_positions_174;
  *driver = gamgi_phys_symmetry_driver_174;
  break;

  case 175:
  *positions = gamgi_phys_symmetry_wyckoff_175;
  *symmetry = gamgi_phys_symmetry_positions_175;
  *driver = gamgi_phys_symmetry_driver_175;
  break;

  case 176:
  *positions = gamgi_phys_symmetry_wyckoff_176;
  *symmetry = gamgi_phys_symmetry_positions_176;
  *driver = gamgi_phys_symmetry_driver_176;
  break;

  case 177:
  *positions = gamgi_phys_symmetry_wyckoff_177;
  *symmetry = gamgi_phys_symmetry_positions_177;
  *driver = gamgi_phys_symmetry_driver_177;
  break;

  case 178:
  *positions = gamgi_phys_symmetry_wyckoff_178;
  *symmetry = gamgi_phys_symmetry_positions_178;
  *driver = gamgi_phys_symmetry_driver_178;
  break;

  case 179:
  *positions = gamgi_phys_symmetry_wyckoff_179;
  *symmetry = gamgi_phys_symmetry_positions_179;
  *driver = gamgi_phys_symmetry_driver_179;
  break;

  case 180:
  *positions = gamgi_phys_symmetry_wyckoff_180;
  *symmetry = gamgi_phys_symmetry_positions_180;
  *driver = gamgi_phys_symmetry_driver_180;
  break;

  case 181:
  *positions = gamgi_phys_symmetry_wyckoff_181;
  *symmetry = gamgi_phys_symmetry_positions_181;
  *driver = gamgi_phys_symmetry_driver_181;
  break;

  case 182:
  *positions = gamgi_phys_symmetry_wyckoff_182;
  *symmetry = gamgi_phys_symmetry_positions_182;
  *driver = gamgi_phys_symmetry_driver_182;
  break;

  case 183:
  *positions = gamgi_phys_symmetry_wyckoff_183;
  *symmetry = gamgi_phys_symmetry_positions_183;
  *driver = gamgi_phys_symmetry_driver_183;
  break;

  case 184:
  *positions = gamgi_phys_symmetry_wyckoff_184;
  *symmetry = gamgi_phys_symmetry_positions_184;
  *driver = gamgi_phys_symmetry_driver_184;
  break;

  case 185:
  *positions = gamgi_phys_symmetry_wyckoff_185;
  *symmetry = gamgi_phys_symmetry_positions_185;
  *driver = gamgi_phys_symmetry_driver_185;
  break;

  case 186:
  *positions = gamgi_phys_symmetry_wyckoff_186;
  *symmetry = gamgi_phys_symmetry_positions_186;
  *driver = gamgi_phys_symmetry_driver_186;
  break;

  case 187:
  *positions = gamgi_phys_symmetry_wyckoff_187;
  *symmetry = gamgi_phys_symmetry_positions_187;
  *driver = gamgi_phys_symmetry_driver_187;
  break;

  case 188:
  *positions = gamgi_phys_symmetry_wyckoff_188;
  *symmetry = gamgi_phys_symmetry_positions_188;
  *driver = gamgi_phys_symmetry_driver_188;
  break;

  case 189:
  *positions = gamgi_phys_symmetry_wyckoff_189;
  *symmetry = gamgi_phys_symmetry_positions_189;
  *driver = gamgi_phys_symmetry_driver_189;
  break;

  case 190:
  *positions = gamgi_phys_symmetry_wyckoff_190;
  *symmetry = gamgi_phys_symmetry_positions_190;
  *driver = gamgi_phys_symmetry_driver_190;
  break;

  case 191:
  *positions = gamgi_phys_symmetry_wyckoff_191;
  *symmetry = gamgi_phys_symmetry_positions_191;
  *driver = gamgi_phys_symmetry_driver_191;
  break;

  case 192:
  *positions = gamgi_phys_symmetry_wyckoff_192;
  *symmetry = gamgi_phys_symmetry_positions_192;
  *driver = gamgi_phys_symmetry_driver_192;
  break;

  case 193:
  *positions = gamgi_phys_symmetry_wyckoff_193;
  *symmetry = gamgi_phys_symmetry_positions_193;
  *driver = gamgi_phys_symmetry_driver_193;
  break;

  case 194:
  *positions = gamgi_phys_symmetry_wyckoff_194;
  *symmetry = gamgi_phys_symmetry_positions_194;
  *driver = gamgi_phys_symmetry_driver_194;
  break;

  case 195:
  *positions = gamgi_phys_symmetry_wyckoff_195;
  *symmetry = gamgi_phys_symmetry_positions_195;
  *driver = gamgi_phys_symmetry_driver_195;
  break;

  case 196:
  *positions = gamgi_phys_symmetry_wyckoff_196;
  *symmetry = gamgi_phys_symmetry_positions_196;
  *driver = gamgi_phys_symmetry_driver_196;
  break;

  case 197:
  *positions = gamgi_phys_symmetry_wyckoff_197;
  *symmetry = gamgi_phys_symmetry_positions_197;
  *driver = gamgi_phys_symmetry_driver_197;
  break;

  case 198:
  *positions = gamgi_phys_symmetry_wyckoff_198;
  *symmetry = gamgi_phys_symmetry_positions_198;
  *driver = gamgi_phys_symmetry_driver_198;
  break;

  case 199:
  *positions = gamgi_phys_symmetry_wyckoff_199;
  *symmetry = gamgi_phys_symmetry_positions_199;
  *driver = gamgi_phys_symmetry_driver_199;
  break;

  case 200:
  *positions = gamgi_phys_symmetry_wyckoff_200;
  *symmetry = gamgi_phys_symmetry_positions_200;
  *driver = gamgi_phys_symmetry_driver_200;
  break;

  case 201:
  *positions = gamgi_phys_symmetry_wyckoff_201;
  *symmetry = gamgi_phys_symmetry_positions_201;
  *driver = gamgi_phys_symmetry_driver_201;
  break;

  case 202:
  *positions = gamgi_phys_symmetry_wyckoff_202;
  *symmetry = gamgi_phys_symmetry_positions_202;
  *driver = gamgi_phys_symmetry_driver_202;
  break;

  case 203:
  *positions = gamgi_phys_symmetry_wyckoff_203;
  *symmetry = gamgi_phys_symmetry_positions_203;
  *driver = gamgi_phys_symmetry_driver_203;
  break;

  case 204:
  *positions = gamgi_phys_symmetry_wyckoff_204;
  *symmetry = gamgi_phys_symmetry_positions_204;
  *driver = gamgi_phys_symmetry_driver_204;
  break;

  case 205:
  *positions = gamgi_phys_symmetry_wyckoff_205;
  *symmetry = gamgi_phys_symmetry_positions_205;
  *driver = gamgi_phys_symmetry_driver_205;
  break;

  case 206:
  *positions = gamgi_phys_symmetry_wyckoff_206;
  *symmetry = gamgi_phys_symmetry_positions_206;
  *driver = gamgi_phys_symmetry_driver_206;
  break;

  case 207:
  *positions = gamgi_phys_symmetry_wyckoff_207;
  *symmetry = gamgi_phys_symmetry_positions_207;
  *driver = gamgi_phys_symmetry_driver_207;
  break;

  case 208:
  *positions = gamgi_phys_symmetry_wyckoff_208;
  *symmetry = gamgi_phys_symmetry_positions_208;
  *driver = gamgi_phys_symmetry_driver_208;
  break;

  case 209:
  *positions = gamgi_phys_symmetry_wyckoff_209;
  *symmetry = gamgi_phys_symmetry_positions_209;
  *driver = gamgi_phys_symmetry_driver_209;
  break;

  case 210:
  *positions = gamgi_phys_symmetry_wyckoff_210;
  *symmetry = gamgi_phys_symmetry_positions_210;
  *driver = gamgi_phys_symmetry_driver_210;
  break;

  case 211:
  *positions = gamgi_phys_symmetry_wyckoff_211;
  *symmetry = gamgi_phys_symmetry_positions_211;
  *driver = gamgi_phys_symmetry_driver_211;
  break;

  case 212:
  *positions = gamgi_phys_symmetry_wyckoff_212;
  *symmetry = gamgi_phys_symmetry_positions_212;
  *driver = gamgi_phys_symmetry_driver_212;
  break;

  case 213:
  *positions = gamgi_phys_symmetry_wyckoff_213;
  *symmetry = gamgi_phys_symmetry_positions_213;
  *driver = gamgi_phys_symmetry_driver_213;
  break;

  case 214:
  *positions = gamgi_phys_symmetry_wyckoff_214;
  *symmetry = gamgi_phys_symmetry_positions_214;
  *driver = gamgi_phys_symmetry_driver_214;
  break;

  case 215:
  *positions = gamgi_phys_symmetry_wyckoff_215;
  *symmetry = gamgi_phys_symmetry_positions_215;
  *driver = gamgi_phys_symmetry_driver_215;
  break;

  case 216:
  *positions = gamgi_phys_symmetry_wyckoff_216;
  *symmetry = gamgi_phys_symmetry_positions_216;
  *driver = gamgi_phys_symmetry_driver_216;
  break;

  case 217:
  *positions = gamgi_phys_symmetry_wyckoff_217;
  *symmetry = gamgi_phys_symmetry_positions_217;
  *driver = gamgi_phys_symmetry_driver_217;
  break;

  case 218:
  *positions = gamgi_phys_symmetry_wyckoff_218;
  *symmetry = gamgi_phys_symmetry_positions_218;
  *driver = gamgi_phys_symmetry_driver_218;
  break;

  case 219:
  *positions = gamgi_phys_symmetry_wyckoff_219;
  *symmetry = gamgi_phys_symmetry_positions_219;
  *driver = gamgi_phys_symmetry_driver_219;
  break;

  case 220:
  *positions = gamgi_phys_symmetry_wyckoff_220;
  *symmetry = gamgi_phys_symmetry_positions_220;
  *driver = gamgi_phys_symmetry_driver_220;
  break;

  case 221:
  *positions = gamgi_phys_symmetry_wyckoff_221;
  *symmetry = gamgi_phys_symmetry_positions_221;
  *driver = gamgi_phys_symmetry_driver_221;
  break;

  case 222:
  *positions = gamgi_phys_symmetry_wyckoff_222;
  *symmetry = gamgi_phys_symmetry_positions_222;
  *driver = gamgi_phys_symmetry_driver_222;
  break;

  case 223:
  *positions = gamgi_phys_symmetry_wyckoff_223;
  *symmetry = gamgi_phys_symmetry_positions_223;
  *driver = gamgi_phys_symmetry_driver_223;
  break;

  case 224:
  *positions = gamgi_phys_symmetry_wyckoff_224;
  *symmetry = gamgi_phys_symmetry_positions_224;
  *driver = gamgi_phys_symmetry_driver_224;
  break;

  case 225:
  *positions = gamgi_phys_symmetry_wyckoff_225;
  *symmetry = gamgi_phys_symmetry_positions_225;
  *driver = gamgi_phys_symmetry_driver_225;
  break;

  case 226:
  *positions = gamgi_phys_symmetry_wyckoff_226;
  *symmetry = gamgi_phys_symmetry_positions_226;
  *driver = gamgi_phys_symmetry_driver_226;
  break;

  case 227:
  *positions = gamgi_phys_symmetry_wyckoff_227;
  *symmetry = gamgi_phys_symmetry_positions_227;
  *driver = gamgi_phys_symmetry_driver_227;
  break;

  case 228:
  *positions = gamgi_phys_symmetry_wyckoff_228;
  *symmetry = gamgi_phys_symmetry_positions_228;
  *driver = gamgi_phys_symmetry_driver_228;
  break;

  case 229:
  *positions = gamgi_phys_symmetry_wyckoff_229;
  *symmetry = gamgi_phys_symmetry_positions_229;
  *driver = gamgi_phys_symmetry_driver_229;
  break;

  case 230:
  *positions = gamgi_phys_symmetry_wyckoff_230;
  *symmetry = gamgi_phys_symmetry_positions_230;
  *driver = gamgi_phys_symmetry_driver_230;
  break;
  }
}

int gamgi_phys_space_multiplicity (const int *driver, int site)
{
int start, end;

/******************************************************
 * Get multiplicity (per node) for this Wyckoff site: *
 * get offset for first and last position for this    *
 * site in the wyckoff array and take the diference.  *
 ******************************************************/

static_range (driver, site, &start, &end);

return end - start + 1;
}

char gamgi_phys_space_letter (int n_sites, int site)
{
/*********************************************************************
 * Group 47 uses 27 Wyckoff sites, exceeding the english alphabet by *
 * one, so we use the letter A for the additional site (Internatinal *
 * Tables for Crystallography use the letter alpha). All the other   *
 * groups require only the 26 letters of the english alphabet.       *
 *********************************************************************/

if (n_sites - site == 'z' - 'a' + 1) return 'A';
return n_sites - site + 'a';
}

void gamgi_phys_space_used (const char **wyckoff, 
const int *driver, int site, gamgi_bool *used)
{
int start, end, i;

used[0] = FALSE;
used[1] = FALSE;
used[2] = FALSE;

static_range (driver, site, &start, &end);

for (i = start; i <= end; i++)
  {
  if (strchr (wyckoff[i], 'x') != NULL) used[0] = TRUE;
  if (strchr (wyckoff[i], 'y') != NULL) used[1] = TRUE;
  if (strchr (wyckoff[i], 'z') != NULL) used[2] = TRUE;
  }
}

double *gamgi_phys_space_positions (int site, double *xyz, 
const char **wyckoff, const int *driver, int *n_positions, 
gamgi_window *window)
{
double input[3];
double *positions;
int site_new;
int start, end;
int n_equal;
int offset;
gamgi_bool *equal;
char line[GAMGI_ENGINE_LINE];

/******************************************************
 * indicating no group information (so wyckoff = NULL *
 * and driver = NULL) or using the basis/motif option *
 * (site = 0) implies using no symmetry at all, so in *
 * practise this works exactly as choosing triclinic  *
 * space group 1, with only one site, with symmetry 1 *
 * and only one position "x,y,z", with multiplicity 1 *
 ******************************************************/

if (site == 0 || wyckoff == NULL || driver == NULL)
  {
  wyckoff = gamgi_phys_symmetry_wyckoff_1;
  driver = gamgi_phys_symmetry_driver_1;
  site = 1;
  }

static_range (driver, site, &start, &end);
*n_positions = end - start + 1;

/*********************************************************
 * the positions array returns to the outside, while the *
 * equal array only exists in the scope of this function *
 *********************************************************/

positions = (double *) malloc (3 * (*n_positions) * sizeof (double));
equal = (gamgi_bool *) malloc (3 * (*n_positions) * sizeof (gamgi_bool));

/******************************************************
 * replicate input coordinates according to Wyckoff   *
 * site positions and check if they are all different *
 ******************************************************/

static_generate (xyz, wyckoff, start, *n_positions, positions);
static_compare_one (*n_positions, positions, equal);
n_equal = static_count (*n_positions, positions, equal);

/****************************
 * look for equal positions *
 ****************************/

if (n_equal > 0)
  {
  /*************************************************************
   * try to find a new Wyckoff site (with lower multiplicity   *
   * and higher symmetry) that generates exactly the same true *
   * positions as obtained above for the selected Wyckoff site *
   *************************************************************/

  *n_positions -= n_equal;
  site_new = static_site (input, wyckoff, 
  driver, *n_positions, positions, equal);

  /***********************************
   * show warning dialog with:       *
   * - number of redundant positions *
   * - new Wyckoff site, if it exits *
   ***********************************/

  offset = sprintf (line, "Ignored %d equal position%s", 
  n_equal, (n_equal == 1) ? "" : "s");
  if (site_new > 0)
    sprintf (line + offset, " and changed to site %c", 
    gamgi_phys_space_letter (driver[0], site_new));
  gamgi_gtk_dialog_message_create ("Warning", line, window);
  }
  
free (equal);
return positions;
}
