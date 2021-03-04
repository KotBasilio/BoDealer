/************************************************************
 * WALRUS Cross platform module                    04.02.2021
 *
 ************************************************************/

// main fork
#ifdef _MSC_VER

 // -------------------------------------------------
// Target: Windows, Visual Studio compiler
#define HEADER_CURSES  <conio.h>
#define PLATFORM_GETCH _getch
#define PLATFORM_KBHIT _kbhit

#else // platforms

// -------------------------------------------------
// Target: Unix, gnuC++ compiler
#define HEADER_CURSES  <curses.h>
#define PLATFORM_GETCH getch
#define PLATFORM_KBHIT kbhit

int kbhit(void);

#endif // platforms

