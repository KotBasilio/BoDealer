/************************************************************
 * Walrus project -- tasks definitions only        29.11.2020
 * --- TASKS, pick only one define of the list below
 ************************************************************/

// Bidding decision one-sided:
//#define SEMANTIC_RED55_KINGS_PART_15_16
//#define SEMANTIC_AUG_MULTI_VUL
//#define SEMANTIC_SEPT_MAJORS54_18HCP
//#define SEMANTIC_NOV_VOIDWOOD
//#define SEMANTIC_NOV_64_AS_TWO_SUITER
//#define SEMANTIC_IMPS_ACCEPT_3NT_ON_SPADE_FIT
//#define SEMANTIC_DEC_12_2425

// Bidding decision competitive:
//#define SEMANTIC_JULY_AUTO_FITO_PLANKTON
//#define SEMANTIC_JUNE_GAMBLE_OR_5C
//#define SEMANTIC_TRICOLOR_STRONG
//#define SEMANTIC_AUG_SPLIT_FIT
//#define SEMANTIC_NOV_DBL_ON_3NT
//#define SEMANTIC_NOV_BID_6C_OR_DBL_4S
#define SEMANTIC_DEC_BID_5H_OR_DBL_4S

// Opening lead:
//#define SEMANTIC_JUNE_MAX_5D_LEAD
//#define SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
//#define SEMANTIC_JUNE_LEAD_3343
//#define SEMANTIC_AUG_LEAD_VS_3H
//#define SEMANTIC_IMPS_LEAD_LEVKOVICH

// One hand tasks:
//#define SEMANTIC_KEYCARDS_10_12
//#define SEMANTIC_SPADE_4_WHEN_1H

// -----------------------------------------------------------------
// --- DESCRIPTIONS
#ifdef SEMANTIC_DEC_12_2425
#define TITLE_VERSION  "Fix a 12 hcp with no fit;\n1s 1NT\npass\nQuestion: did we miss the 3NT? Task"
#define SEEK_BIDDING_LEVEL
#endif // SEMANTIC_DEC_12_2425

// ---
#ifdef SEMANTIC_IMPS_ACCEPT_3NT_ON_SPADE_FIT
#define TITLE_VERSION  "Fix a 8 hcp with split/fit;\n1s 2h(split)\n3s ??\nQuestion: partscore 3s or a game 3NT? Task"
#define SEEK_DENOMINATION
#endif // SEMANTIC_IMPS_ACCEPT_3NT_ON_SPADE_FIT

// ---
#ifdef SEMANTIC_NOV_64_AS_TWO_SUITER
#define TITLE_VERSION  "Fix a 10 hcp 6-4;\n2s 3s\n??\nQuestion: partscore or a game? v2.0"
#define SEEK_BIDDING_LEVEL
#endif // SEMANTIC_NOV_64_AS_TWO_SUITER

// ---
#ifdef SEMANTIC_NOV_DBL_ON_3NT
#define TITLE_VERSION  "Fix some even values 8hcp;\nOpps bid:\np-1c\n1d-1nt(17-21)\n2h 3NT\n\nQuestion: maybe double that? v2.0"
#define SEEK_DECISION_OVER_DOUBLE
#define SHOW_OPP_RESULTS
#endif // SEMANTIC_NOV_DBL_ON_3NT

// ---
#ifdef SEMANTIC_NOV_BID_6C_OR_DBL_4S
//#define TITLE_VERSION  "Fix 17 hcp hand;\n1c-(1s)-dbl-(3s)\n-3NT-(4s)-dbl-(p)-??\nAssume partner 12+\nQuestion: dbl, 5 clubs / 6 clubs? Ver 2.0 "
//#define TITLE_VERSION  "Fix 17 hcp hand;\n1c-(1s)-dbl-(3s)\n-3NT-(4s)-dbl-(p)-??\nAssume partner (10-11)\nQuestion: dbl, 5 clubs / 6 clubs? Ver 2.0 "
#define TITLE_VERSION  "Fix 17 hcp hand;\n1c-(1s)-dbl-(3s)\n-3NT-(4s)-dbl-(p)-??\nAssume partner 13+\nQuestion: dbl, 5 clubs / 6 clubs? Ver 2.0 "
#define SEEK_BIDDING_LEVEL
#define SCORE_OPP_CONTRACT
#define OC_TRUMPS       SOL_SPADES
#define OC_ON_LEAD      SOUTH
#endif 

// ---
#ifdef SEMANTIC_DEC_BID_5H_OR_DBL_4S
#define TITLE_VERSION  "Fix heart-top-7 hand;\n1c-(1s)-pass-(3s)\n-4h-(4s)-dbl-(p)-??\nQuestion: dbl or 5h? Ver 2.0 "
#define SEEK_BIDDING_LEVEL
#define SCORE_OPP_CONTRACT
#define OC_TRUMPS       SOL_SPADES
#define OC_ON_LEAD      SOUTH
#endif // SEMANTIC_DEC_BID_5H_OR_DBL_4S

// ---
#ifdef SEMANTIC_NOV_VOIDWOOD
#define TITLE_VERSION  "Fix a 3730 three aces;\n1nt(11-13) 2d\n2h 5c -- opp doubled that\nXX(zero of 5) ??\nQuestion: a game or a slam? v2.0"
#define SEEK_BIDDING_LEVEL
#define UNPLAYABLE_ONE_OF  6    // adjust to singleton king offside -- go down in 1/6 of such cases
#endif // SEMANTIC_NOV_VOIDWOOD

// ---
#ifdef SEMANTIC_SEPT_MAJORS54_18HCP
#define TITLE_VERSION  "Fix a 18 hcp;\n1c 1d\n1s 1NT\n2h 3h\nQuestion: partscore or a game? v2.0"
#define SEEK_BIDDING_LEVEL
#endif // SEMANTIC_SEPT_MAJORS54_18HCP

// ---
#ifdef SEMANTIC_SPADE_4_WHEN_1H
#define TITLE_VERSION  "Part opened 1H. We hold ~4333 with 4s;\nQuestion: how often part has 4+ spades? Ver 2.0 "
#define PARTNER_HAND_TASK
#define PERCENTAGES_IN_ANSWER_ROW
#define ANSWER_ROW_IDX  1
#endif // SEMANTIC_SPADE_4_WHEN_1H

// ---
#ifdef SEMANTIC_AUG_LEAD_VS_3H
#define TITLE_VERSION  "Fix a hand with an ace;\n....s\nQuestion: what to lead?\nVer 2.0 "
#define SEEK_OPENING_LEAD
#endif 

// ---
#ifdef SEMANTIC_IMPS_LEAD_LEVKOVICH
#define TITLE_VERSION  "Fix a hand with an 3-4 majors;\n3d-p-3NTs\nQuestion: what to lead?\nVer 2.0 "
#define SEEK_OPENING_LEAD
#define PERCENTAGES_IN_ANSWER_ROW
#endif 

// -------------------------------------------------------- 
#ifdef SEMANTIC_KEYCARDS_10_12
#define TITLE_VERSION  "Abstract 10-12 with 4+ major;\nQuestion: how keycards are distributed? Ver 2.0 "
#define SINGLE_HAND_TASK
#define PERCENTAGES_IN_ANSWER_ROW
#endif 

// -------------------------------------------------------- 
#ifdef SEMANTIC_JULY_AUTO_FITO_PLANKTON
#define TITLE_VERSION  "Fix 7 hcp hand;\n(1s)-2h-(2s)-3h\n(p)-p-(3s)-??\nQuestion: pass/dbl or a game? Ver 2.0 "
#define SEEK_BIDDING_LEVEL
#define SCORE_OPP_CONTRACT
#define OC_TRUMPS       SOL_SPADES
#define OC_ON_LEAD      SOUTH
#endif 

// -------------------------------------------------------- JUNE_MAX_5D_LEAD
#ifdef SEMANTIC_JUNE_MAX_5D_LEAD
#define TITLE_VERSION  "Fix a strong hand;\n(p)-p-(1d)-1h\n(3d)-3h-(p)-4h\n(p)-p-(5d)-X-all pass\nQuestion: what to lead?\nVer 2.0 "
#define SEEK_OPENING_LEAD
#endif 

// -------------------------------------------------------- JUNE_LEAD_3343
#ifdef SEMANTIC_JUNE_LEAD_3343
#define TITLE_VERSION  "Fix a weak hand;\np-(p)-p-(1s)\np-(1NT)-p-(3NT)-all pass\nQuestion: what to lead?\nVer 2.0 "
#define SEEK_OPENING_LEAD
#endif 

// -------------------------------------------------------- JUNE_GAMBLE_OR_5C
#ifdef SEMANTIC_JUNE_GAMBLE_OR_5C
   #define TITLE_VERSION  "Fix 3307 fourth hand;\n(2h)-p-(p)-??\nQuestion: 3NT, partscore or 5C? Ver 2.0 "
   #define SEEK_DENOMINATION
#endif 

// -------------------------------------------------------- JUNE_ZAKHAROVY_PREC_3NT
#ifdef SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
   #define TITLE_VERSION  "Fix a weak hand;\n(2c)-p-(3NT)-all pass\nQuestion: what to lead?\nVer 2.0 "
   #define SEEK_OPENING_LEAD
#endif 

// -------------------------------------------------------- TRICOLOR_STRONG
#ifdef SEMANTIC_TRICOLOR_STRONG
   #define TITLE_VERSION  "Fix tricolor opening;\np-(p)-1c-(1d)\np-(p)-dbl-(2d)\n2h-(p)-??\nQuestion: partscore or a game? Ver 2.0 "
   #define SEEK_BIDDING_LEVEL
#endif 

// -------------------------------------------------------- RED55_KINGS_PART_15_16
#ifdef SEMANTIC_RED55_KINGS_PART_15_16
   #define TITLE_VERSION  "Fix 55 responder; 1NT-2d-3d-?? Question: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

#ifdef SEMANTIC_AUG_SPLIT_FIT
	#define TITLE_VERSION  "Fix 6-4 responder; (2d)-2s-(dbl)-?? Question: partscore or a game? v2.0"
	#define SEEK_BIDDING_LEVEL
#endif // SEMANTIC_AUG_SPLIT_FIT

#ifdef SEMANTIC_AUG_MULTI_VUL
	#define TITLE_VERSION  "Fix a 15 hcp;\n....s\nQuestion: partscore or a game? v2.0"
	#define SEEK_BIDDING_LEVEL
#endif // SEMANTIC_AUG_MULTI_VUL

#ifdef SCORE_OPP_CONTRACT
   #define SHOW_OPP_RESULTS
#endif 

// -----------------------------------------------------------------
// --- COMMON
typedef unsigned char UCHAR;
typedef unsigned short int u16;
typedef unsigned int uint;
typedef unsigned long long u64;
typedef signed long long s64;
const uint SOURCE_CARDS_COUNT = 52;
#define SBITS_LAYOUT_TWELVE

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
// --- Derivatives

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

// -----------------------------------------------------------------
// --- Iterations count
#ifdef _DEBUG
   //const uint MAX_ITERATION = 20*1000000;// 20 mln 
   //const uint MAX_ITERATION = 1000000;// 1 mln 
   const uint MAX_ITERATION = 100000;// 0.1 mln
   //const uint MAX_ITERATION = 10000;
   const uint MAX_TASKS_TO_SOLVE = 10240;
   //#define SKIP_HELPERS
#else
   //const uint MAX_ITERATION = 4001000000;// 4 mlrd + 1 millon gratis
   //const uint MAX_ITERATION = 3001000000;// 3 mlrd + 1 millon gratis
   //const uint MAX_ITERATION = 2001000000;// 2 mlrd + 1 millon gratis
   //const uint MAX_ITERATION = 1001000000;// 1 mlrd + 1 millon gratis
   //const uint MAX_ITERATION = 501000000;// half of mlrd
   //const uint MAX_ITERATION = 330000000;// third of mlrd
   //const uint MAX_ITERATION = 251000000;// quarter mlrd
   const uint MAX_ITERATION = 101000000;// 101 mln
   const uint MAX_TASKS_TO_SOLVE = 4*1000*1000;
#endif

