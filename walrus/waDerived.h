/************************************************************
 * Walrus project -- derived definitions and constants   13.08.2022
 ************************************************************/

 // what hand is fixed? (if any)
#ifdef SEEK_DENOMINATION
   #define SCORE_THE_OTHER_CONTRACT
   #define THE_OTHER_IS_OURS
#endif

#ifdef DETAILED_LEADS
   #define SEM_ORBITING_FIXED_HAND
   #define FIXED_HAND_WEST
#endif

// Algorithm details --- how to solve and what to seek
#ifdef SEEK_MAGIC_FLY
   #define FIXED_HAND_NORTH
   #define ON_LEAD_INTERROGATED   cards.first
   #define SHOW_MY_FLY_RESULTS
#endif

#ifdef SEEK_SACRIFICE_DECISION
   #define SHOW_OPPS_ON_PASS_ONLY
   #define SEEK_DECISION_COMPETE
#endif

#ifdef SEEK_DECISION_BID_OR_DOUBLE
   #define SHOW_OPPS_ON_DOUBLE_ONLY
   #define SEEK_DECISION_COMPETE
#endif

#ifdef SEEK_DECISION_COMPETE
   #define SCORE_THE_OTHER_CONTRACT
   #define SHOW_SACRIFICE_RESULTS
   #define FIXED_HAND_NORTH
#endif

#ifdef SCORE_THE_OTHER_CONTRACT
   #define ON_LEAD_INTERROGATED   config.secondary.first
   #ifdef THE_OTHER_IS_OURS
      #define SHOW_OUR_OTHER
   #else
      #define SHOW_OPP_RESULTS
   #endif
#endif

#ifndef ON_LEAD_INTERROGATED
   #define ON_LEAD_INTERROGATED   WEST
#endif

// tasks without a fixed hand (no specifics)

// User interface --- how to display

// -----------------------------------------------------------------
// more resolutions on a fixed hand
#ifdef SEEK_BIDDING_LEVEL
   #define FIXED_HAND_NORTH
#endif
#ifdef SEM_ONE_SIDED_DENOMINATION
   #define FIXED_HAND_NORTH
#endif

// -----------------------------------------------------------------
// common constants, biggest symmetry in bridge
const uint SYMM = 13;

// -----------------------------------------------------------------
// --- Derivative constants
const uint SYMM2 = SYMM * 2;
const uint SYMM3 = SYMM * 3;
const uint SOURCE_CARDS_COUNT = SYMM * 4;
#if defined(FIXED_HAND_NORTH) || defined(FIXED_HAND_WEST)
   const uint REMOVED_CARDS_COUNT = SYMM;
#else
   const uint REMOVED_CARDS_COUNT = 0;
#endif // any FIXED_HAND_...
const uint ACTUAL_CARDS_COUNT = SOURCE_CARDS_COUNT - REMOVED_CARDS_COUNT;

#ifdef PERCENTAGES_IN_ANSWER_ROW
   #ifndef ANSWER_ROW_IDX
      #define ANSWER_ROW_IDX  0
   #endif
#endif // PERCENTAGES_IN_ANSWER_ROW

// -----------------------------------------------------------------
// --- Iterations count
typedef u64 ucell;
#ifdef _DEBUG
   //#define SKIP_HELPERS
   //#define SOLVE_ONE_BY_ONE
   const ucell WALRUS_PERF_FRACTION = 2000;
#else
   const ucell WALRUS_PERF_FRACTION = 10;
#endif

//const ucell MAX_ITERATION = 6001000000LL;// 6 mlrd (u64, not max)
//const ucell MAX_ITERATION = 3001000000;// 3 mlrd 
//const ucell MAX_ITERATION = 501000000;// a half of mlrd
//const ucell MAX_ITERATION = 333000000;// a third of mlrd
const ucell MAX_ITERATION = 251000000;// a quarter of mlrd
//const ucell MAX_ITERATION = 101000000;// 101 mln
//const ucell MAX_ITERATION = 20 * 1000000;// 20 mln 
//const ucell MAX_ITERATION = 5000000;// 5 mln 
//const ucell MAX_ITERATION = 1000000;// 1 mln 
//const ucell MAX_ITERATION = 100000;// 0.1 mln
//const ucell MAX_ITERATION = 3000;

constexpr ucell ClipByPerformance(ucell lim, ucell a)
{
   return (a < lim / WALRUS_PERF_FRACTION) ? a : lim / WALRUS_PERF_FRACTION;
}
const ucell ADDITION_STEP_ITERATIONS = ClipByPerformance(MAX_ITERATION, 501000000);
