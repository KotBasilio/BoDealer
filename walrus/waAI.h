/************************************************************
 * Walrus AI                                       27.12.2019
 *
 ************************************************************/

#include "waLayout.h"

// A task version when we need to store only two hands
// It's useful for:
// -- bidding decisions.      Then the fixed hand is North
// -- opening lead decisions. Then the fixed hand is West
struct WaTask2 
{
   SplitBits partner;
   SplitBits oneOpp;

   WaTask2() {}
   void Init(SplitBits& a, SplitBits& b) { partner = a; oneOpp = b; }
   void Init(twContext* lay);
};

// A task version when store CodedTricks as well -- so we can refilter later
struct WaTaskStray : public WaTask2 {
   u64 tricks = 0;// TODO some coded arithmetics like in SplitBits

   WaTaskStray() : WaTask2() {}
   void Init(SplitBits& a, SplitBits& b) { partner = a; oneOpp = b; }
   void Init(twContext* lay);
};

// a task version with 3 hands stored
#ifdef _DEBUG
   #define DEBUG_UNEXPECTED printf("\nwtf?")
#else
   #define DEBUG_UNEXPECTED
#endif
struct WaTask3
{
   SplitBits north, east, south;

   WaTask3() {}
   void Init(SplitBits& a, SplitBits& b);
   void Init(twContext* lay);
};

// most of the program doesn't care which task version is used
#ifdef FOUR_HANDS_TASK
   typedef WaTask3 WaTask;
#else
   typedef WaTask2 WaTask;
#endif

// other used-through definitions
#define NORTH    0
#define EAST     1
#define SOUTH    2
#define WEST     3

#define SOL_SPADES   0
#define SOL_HEARTS   1
#define SOL_DIAMONDS 2
#define SOL_CLUBS    3
#define SOL_NOTRUMP  4

#define SPD SOL_SPADES
#define HRT SOL_HEARTS
#define DMD SOL_DIAMONDS
#define CLB SOL_CLUBS

// Double-dummy solver integration
void HandleErrorDDS(struct deal &dl, int res);
void sample_main_SolveBoard();
void sample_main_SolveBoard_S1();
void sample_main_PlayBin();
void sample_main_JK_Solve();
