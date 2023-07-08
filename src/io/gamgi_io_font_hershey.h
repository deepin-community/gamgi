/****************************************************************************
 *
 * $GAMGI/src/io/gamgi_io_font_hershey.h
 *
 * The Hershey fonts in this file were taken from the GNU package 
 * plotutils, including the arrays _occidental_hershey_glyphs[] and 
 * _oriental_hershey_glyphs[]. The first array includes the standard
 * (`occidental') Hershey glyphs digitized by Dr. Allen V. Hershey, some
 * glyphs due to other people, and his Japanese Hiragana and Katakana
 * glyphs. The second array includes his digitizations of Japanese
 * ideographic symbols (Kanji glyphs).  It originally included the Hiragana
 * and Katakana glyphs as well, but they were moved to the first array
 * because some people may not wish to include Kanji support.
 *
 * Dr. Hershey digitized the glyphs c. 1967, at what is now the U.S. Naval
 * Surface Weapons Center in Dahlgren, Virginia.  For many years he
 * distributed copies of the glyphs, and his typographic software, on
 * magnetic tape.  Over 120 copies of the tape were distributed.  There
 * have been many other distributions of the glyphs.  In the 1970's they
 * were incorporated, not always with attribution, in several commercial
 * plotting packages.  They were first freely distributed in 1985(?), by
 * being posted to Usenet (to vol. 4 of mod.sources) by Pete Holzmann
 * <pete@xc.org>, then at Octopus Enterprises.  In the 1980's the glyphs
 * were incorporated in at least two freeware plotting programs, Nelson
 * Beebe's PLOT79 and Tim Pearson's PGPLOT. The latter is still available
 * (see http://astro.caltech.edu/~tjp/pgplot/).
 *
 * The _occidental_hershey_glyphs[] array was constructed from
 * releases of the Hershey glyphs that are later than the one described in
 * the 1976 NBS publication above. For example, Fig. 15 of that work
 * portrays 43 additional glyphs digitized by Norman Wolcott of the NBS;
 * they are included here.  See the file ./doc/hershey-number for details
 * on how the array was assembled.  The original Hershey glyphs, including
 * the 43 Wolcott glyphs, are stored in the 0..3999 slots of the array.
 *
 * The _occidental_hershey_glyphs[] array also includes non-Hershey glyphs
 * taken from various sources.  All post-Hershey glyphs have been placed in
 * slots 4000..4194.  This includes a few glyphs taken from the UGS (the
 * Unified Graphics System, developed by Bob Beach at SLAC; see Computer
 * Graphics, Fall 1974, pp. 22-23).  As well, it includes some freeware
 * glyphs developed by Thomas Wolff <wolff@inf.fu-berlin.de> and
 * distributed as part of the Ghostscript distribution.  The Beach and
 * Wolff glyphs are used in the Hershey Symbol fonts.  Locally developed
 * glyphs (accented characters, in particular) have been added too.
 *
 * The Japanese Hiragana and Katakana glyphs digitized by Dr. Hershey,
 * formerly located among the `oriental' glyphs, are now located in slots
 * 4195..4399 of the _occidental_hershey_glyphs[] array.
 *
 * The format of the glyphs in the _occidental_hershey_glyphs[] and
 * _oriental_hershey_glyphs[] arrays is the format in which the Hershey
 * glyphs were distributed to mod.sources in 1985(?) by Pete Holzmann, and
 * is due to Jim Hurt, then at Cognition Inc.
 *
 * The standard hardcopy reference for the occidental Hershey glyphs, which
 * tabulates and displays them, is:
 *
 * @TechReport{Wolcott76,
 *   author =       {Norman M. Wolcott and Joseph Hilsenrath},
 *   title =        {A Contribution to Computer Typesetting Techniques:
 *                   Tables of Coordinates for {Hershey's} Repertory of
 *                   Occidental Type Fonts and Graphic Symbols},
 *   institution =  {U.S. National Bureau of Standards},
 *   year =         {1976},
 *   month =        {April},
 *   type =         {Special Publication},
 *   number =       {424},
 *   OPTnote =      {US NTIS stock number PB251845}}
 *
 * This publication is available from the NTIS (US National Technical
 * Information Service, +1 703 487 4650).  The NTIS stock number is given
 * above.  See also Dr. Hershey's original TR, which describes the
 * alphabets from which the glyphs were taken and gives information on the
 * oriental glyphs:
 *
 * @TechReport{Hershey67,
 *   author =    {Allen V. Hershey},
 *   title =     {Calligraphy for Computers},
 *   institution =  {U.S. Naval Weapons Laboratory},
 *   address =   {Dahlgren, VA},
 *   year =      {1967},
 *   type =      {Report},
 *   number =    {TR--2101},
 *   month =     {Aug},
 *   OPTnote =   {US NTIS stock number AD662398}}
 *
 */

extern const float gamgi_io_font_hershey_sans_points[GAMGI_IO_POINTS_SANS];

extern const float gamgi_io_font_hershey_sans_bold_points[GAMGI_IO_POINTS_SANS_BOLD];

extern const float gamgi_io_font_hershey_serif_points[GAMGI_IO_POINTS_SERIF];

extern const float gamgi_io_font_hershey_serif_bold_points[GAMGI_IO_POINTS_SERIF_BOLD];

extern const float gamgi_io_font_hershey_script_points[GAMGI_IO_POINTS_SCRIPT];

extern const float gamgi_io_font_hershey_script_bold_points[GAMGI_IO_POINTS_SCRIPT_BOLD];

extern const float gamgi_io_font_hershey_gothic_english_points[GAMGI_IO_POINTS_GOTHIC_ENGLISH];

extern const float gamgi_io_font_hershey_gothic_german_points[GAMGI_IO_POINTS_GOTHIC_GERMAN];

extern const float gamgi_io_font_hershey_gothic_italian_points[GAMGI_IO_POINTS_GOTHIC_ITALIAN];

extern const float gamgi_io_font_hershey_symbol_sans_points[GAMGI_IO_POINTS_SYMBOL_SANS];

extern const float gamgi_io_font_hershey_symbol_serif_points[GAMGI_IO_POINTS_SYMBOL_SERIF];

extern const float gamgi_io_font_hershey_symbol_serif_bold_points[GAMGI_IO_POINTS_SYMBOL_SERIF_BOLD];

extern const int gamgi_io_font_hershey_sans_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_sans_bold_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_serif_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_serif_bold_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_script_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_script_bold_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_gothic_english_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_gothic_german_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_gothic_italian_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_symbol_sans_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_symbol_serif_driver [GAMGI_IO_ASCII_RANGE];

extern const int gamgi_io_font_hershey_symbol_serif_bold_driver [GAMGI_IO_ASCII_RANGE];

/*************
 * Rationale *
 *************

 *************
 * Rationale *
 *************/

