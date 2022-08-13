/************************************************************
 * Walrus project -- derived definitions and consts   13.08.2022
 ************************************************************/

// -----------------------------------------------------------------
// --- HOW TO SOLVE AND DISPLAY
#ifdef SCORE_OPP_CONTRACT
   #define SHOW_OPP_RESULTS
   #define RE_SOLVE_HANDLED_CHUNK
#endif

#ifndef SHOW_OPP_RESULTS
   #define SHOW_PARTSCORE
#endif

// how to filter (personal opinions)
//#define OPINION_BASHA
//#define ACCEPT_INVITE

// -----------------------------------------------------------------
// --- WHAT TO SEEK, setup or miss a few options
// algorithm details and output may also depend on that

#ifdef SEEK_DECISION_OVER_DOUBLE
   #define FIXED_HAND_WEST
#endif

#ifdef SEEK_BIDDING_LEVEL
   #define FIXED_HAND_NORTH
#endif

#ifdef SEEK_DENOMINATION
   #define FIXED_HAND_NORTH
#endif

#ifdef SEEK_OPENING_LEAD
   #define DETAILED_LEADS
   #define FIXED_HAND_WEST
#endif

#ifdef SINGLE_HAND_TASK
   // no lead definition
   // no fixed hand definition
#endif

#ifdef PARTNER_HAND_TASK
   #define FIXED_HAND_NORTH
#endif

// -----------------------------------------------------------------
// --- Derivative constants
const uint SOURCE_CARDS_COUNT = 52;

#if defined(FIXED_HAND_NORTH) || defined(FIXED_HAND_WEST)
   const uint REMOVED_CARDS_COUNT = 13;
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



