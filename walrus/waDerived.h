/************************************************************
 * Walrus project -- derived definitions and constants   13.08.2022
 ************************************************************/

// Algorithm details --- how to solve and what to seek
#ifdef SEEK_MAGIC_FLY
   #define FIXED_HAND_NORTH
   #define SOLVE_TWICE_HANDLED_CHUNK
   #define TWICE_TRUMPS            SOL_NOTRUMP
   #define TWICE_ON_LEAD_INSPECT   cards.first
   #define TWICE_ON_LEAD_CHUNK     bo.deals[i].first
   #define SHOW_MY_FLY_RESULTS
#endif

#ifdef SEEK_SACRIFICE_DECISION
   #define SEEK_BIDDING_LEVEL
   #define SCORE_THE_OTHER_CONTRACT
   #define SHOW_SACRIFICE_RESULTS
#endif

#ifdef SCORE_THE_OTHER_CONTRACT
   #define SOLVE_TWICE_HANDLED_CHUNK
   #define TWICE_TRUMPS            OC_TRUMPS
   #define TWICE_ON_LEAD_INSPECT   OC_ON_LEAD
   #define TWICE_ON_LEAD_CHUNK     OC_ON_LEAD
   #ifdef THE_OTHER_IS_OURS
      #define SHOW_OUR_OTHER
   #else
      #define SHOW_OPP_RESULTS
   #endif
#endif

#ifndef TWICE_ON_LEAD_INSPECT
   #define TWICE_ON_LEAD_INSPECT   SOL_CLUBS
   #define TWICE_ON_LEAD_CHUNK     NORTH
#endif

#ifdef SEEK_DECISION_BID_OR_DOUBLE
   #define SEEK_BIDDING_LEVEL
   #define SHOW_SACRIFICE_RESULTS
#endif

// what hand is fixed? (if any)
#ifdef SEEK_DECISION_WHETHER_DOUBLE
   #define FIXED_HAND_WEST
#endif

#ifdef SEEK_BIDDING_LEVEL
   #define FIXED_HAND_NORTH
#endif

#ifdef SEEK_DENOMINATION
   #define FIXED_HAND_NORTH
#endif

#ifdef PARTNER_HAND_TASK
   #define FIXED_HAND_NORTH
#endif

#ifdef SEEK_OPENING_LEAD
   #define DETAILED_LEADS
   #define FIXED_HAND_WEST
#endif

// tasks without a fixed hand (no specifics)

// User interface --- how to display
#ifndef IO_SHIFT_FOR_EXTRA_MARKS
   #define IO_SHIFT_FOR_EXTRA_MARKS  21
#endif

#define IO_LAYOUT_3_0
#define IO_ROW_HCP_START 3
#define IO_ROW_FILTERING (3  + IO_SHIFT_FOR_EXTRA_MARKS)
#define IO_ROW_SELECTED  (11 + IO_SHIFT_FOR_EXTRA_MARKS)
#define IO_SHOW_MINI_FILTERING

#ifdef IO_HCP_MIN
   #if IO_HCP_MIN == IO_HCP_MAX
      #define  IO_DISPLAY_CONTROLS_SPLIT
   #endif
#else
   #define IO_HCP_MIN  25
   #define IO_HCP_MAX  25
#endif

#ifndef SHOW_OPP_RESULTS
   #define SHOW_PARTSCORE_STATLINE
   #define IO_SHOW_HCP_CTRL_SPLIT
#endif

// -----------------------------------------------------------------
// personal opinions on how to filter

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

// DOC: solutions parameter
// 1 -- Find the maximum number of tricks for the side to play. Return only one of the optimum cards and its score.
// 2 -- Find the maximum number of tricks for the side to play. Return all optimum cards and their scores.
// 3 -- Return all cards that can be legally played, with their scores in descending order.
#ifdef DETAILED_LEADS
   #define PARAM_SOLUTIONS_DDS   3
#else
   #define PARAM_SOLUTIONS_DDS   1
#endif // DETAILED_LEADS

// -----------------------------------------------------------------
// --- Iterations count
typedef u64 ucell;
#ifdef _DEBUG
   const uint WALRUS_CHUNK_SIZE = 20;
   const ucell WALRUS_PERF_FRACTION = 2000;
   //#define SKIP_HELPERS
   //#define SOLVE_ONE_BY_ONE
#else
   const uint WALRUS_CHUNK_SIZE = 100;
   const ucell WALRUS_PERF_FRACTION = 10;
#endif

//const ucell MAX_ITERATION = 6001000000LL;// 6 mlrd (u64)
const ucell MAX_ITERATION = 3001000000;// 3 mlrd 
//const ucell MAX_ITERATION = 501000000;// a half of mlrd
//const ucell MAX_ITERATION = 333000000;// a third of mlrd
//const ucell MAX_ITERATION = 251000000;// a quarter of mlrd
//const ucell MAX_ITERATION = 101000000;// 101 mln
//const ucell MAX_ITERATION = 20 * 1000000;// 20 mln 
//const ucell MAX_ITERATION = 5000000;// 5 mln 
//const ucell MAX_ITERATION = 1000000;// 1 mln 
//const ucell MAX_ITERATION = 100000;// 0.1 mln
//const ucell MAX_ITERATION = 3000;

const uint MAX_TASKS_TO_SOLVE = 200 * 1000;
//const uint AIM_TASKS_COUNT = MAX_TASKS_TO_SOLVE / 2;
//const uint AIM_TASKS_COUNT = 50 * 1000;
//const uint AIM_TASKS_COUNT = 20*1000;
const uint AIM_TASKS_COUNT = 5*1000;
//const uint AIM_TASKS_COUNT = 2*1000;

constexpr ucell ClipByPerformance(ucell lim, ucell a)
{
   return (a < lim / WALRUS_PERF_FRACTION) ? a : lim / WALRUS_PERF_FRACTION;
}
const ucell ADDITION_STEP_ITERATIONS = ClipByPerformance(MAX_ITERATION, 501000000);
