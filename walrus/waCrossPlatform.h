/************************************************************
 * WALRUS Cross platform module                    04.02.2021
 *
 ************************************************************/
#ifndef WALRUS_CROSS_PFM
#define WALRUS_CROSS_PFM

// main fork
#ifdef _MSC_VER

 // -------------------------------------------------
// Target: Windows, Visual Studio compiler
#define HEADER_CURSES    <conio.h>
#define HEADER_SLEEP     <Windows.h>
#define HEADER_THREADS   <process.h>

#define PLATFORM_GETCH _getch
#define PLATFORM_KBHIT _kbhit
#define PLATFORM_SLEEP(TIME)  Sleep(TIME)
#define PFM_THREAD_RETTYPE  void
#define PFM_THREAD_RETVAL 
#define PLATFORM_BEGIN_THREAD(FOO, ARG)  _beginthread(FOO, 0, ARG)

//#define WIN_DETECT_PATH

#else // platforms

// -------------------------------------------------
// Target: Unix, gnuC++ compiler
#define HEADER_CURSES    <curses.h>
#define HEADER_SLEEP     <unistd.h>
#define HEADER_THREADS   <pthread.h>

#define PLATFORM_GETCH getch
#define PLATFORM_KBHIT kbhit
#define PLATFORM_SLEEP(TIME)  sleep(TIME)

#define PFM_THREAD_RETTYPE  void*
#define PFM_THREAD_RETVAL   0
#define PLATFORM_BEGIN_THREAD(FOO, ARG)                   \
   {                                                      \
      pthread_t thread1;                                  \
      pthread_create(&thread1, NULL, FOO, (void*)(ARG));  \
   }


typedef unsigned int DWORD;
typedef unsigned int guint32;

int kbhit(void);

#define MAXUINT32	((guint32) 0xffffffff)
#define SUCCESS               0

#define __max(a,b) \
       ({ typeof (a) _a = (a); \
           typeof (b) _b = (b); \
         _a > _b ? _a : _b; })

#endif // platforms

#include <stdio.h> // printf

// -----------------------------------------------------------------
// --- common types
typedef unsigned char UCHAR;
typedef unsigned short int u16;
typedef unsigned int uint;
typedef unsigned long long u64;
typedef signed long long s64;

#endif //WALRUS_CROSS_PFM