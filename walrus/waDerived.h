/************************************************************
 * Walrus project -- derived definitions and constants   13.08.2022
 ************************************************************/

// Algorithm details:
// -- what hand is fixed, if any
// -- how to solve and what to seek
#ifdef SEEK_DENOMINATION
   #define SCORE_THE_OTHER_CONTRACT
   #define THE_OTHER_IS_OURS
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
   #define FIXED_HAND_NORTH
#endif

// tasks without a fixed hand (no specifics)

// -----------------------------------------------------------------
// common constants, biggest symmetry in bridge
constexpr uint SYMM = 13;
constexpr uint SYMM2 = SYMM * 2;
constexpr uint SYMM3 = SYMM * 3;
constexpr uint SOURCE_CARDS_COUNT = SYMM * 4;

// -----------------------------------------------------------------
// --- Iterations count
typedef u64 ucell;
#ifdef _DEBUG
   //#define SKIP_HELPERS
   //#define SOLVE_ONE_BY_ONE
   constexpr ucell WALRUS_PERF_FRACTION = 2000;
#else
   constexpr ucell WALRUS_PERF_FRACTION = 10;
#endif

//constexpr ucell MAX_ITERATION = 6001000000LL;// 6 mlrd (u64, not max)
//constexpr ucell MAX_ITERATION = 3001000000;// 3 mlrd 
//constexpr ucell MAX_ITERATION = 501000000;// a half of mlrd
//constexpr ucell MAX_ITERATION = 333000000;// a third of mlrd
constexpr ucell MAX_ITERATION = 251000000;// a quarter of mlrd
//constexpr ucell MAX_ITERATION = 101000000;// 101 mln
//constexpr ucell MAX_ITERATION = 20 * 1000000;// 20 mln 
//constexpr ucell MAX_ITERATION = 5000000;// 5 mln 
//constexpr ucell MAX_ITERATION = 1000000;// 1 mln 
//constexpr ucell MAX_ITERATION = 100000;// 0.1 mln
//constexpr ucell MAX_ITERATION = 3000;

constexpr ucell ClipByPerformance(ucell lim, ucell a)
{
   return (a < lim / WALRUS_PERF_FRACTION) ? a : lim / WALRUS_PERF_FRACTION;
}
constexpr ucell ADDITION_STEP_ITERATIONS = ClipByPerformance(MAX_ITERATION, 501000000);
