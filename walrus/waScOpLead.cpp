/************************************************************
 * Walrus project                                        2020
 * Deprecated parts and diagnostics
 *
 ************************************************************/
#include "walrus.h"

// catch conflicting task defines
#ifdef SEM_ONE_SIDED_BIDDING_LEVEL
   #ifdef SEM_ONE_SIDED_DENOMINATION
      #error Conflicting defines SEM_ONE_SIDED_BIDDING_LEVEL and SEM_ONE_SIDED_DENOMINATION. Choose only one.
   #endif
   #ifdef SEM_COMPETITIVE_GENERIC
      #error Conflicting defines SEM_ONE_SIDED_BIDDING_LEVEL and SEM_COMPETITIVE_GENERIC. Choose only one.
   #endif
#endif

#ifdef SEM_ONE_SIDED_DENOMINATION
   #ifdef SEM_COMPETITIVE_GENERIC
      #error Conflicting defines SEM_ONE_SIDED_DENOMINATION and SEM_COMPETITIVE_GENERIC. Choose only one.
   #endif
#endif

// catch old-style task defines
#ifdef INPUT_TRUMPS
   #error Get rid of INPUT_TRUMPS define. Use primary linear scorer.
#endif
#ifdef INPUT_ON_LEAD
   #error Get rid of INPUT_ON_LEAD define. Use primary linear scorer with BY keyword.
#endif

#ifdef OC_TRUMPS
   #error Get rid of OC_TRUMPS define. Use secondary linear scorer.
#endif
#ifdef OC_ON_LEAD
   #error Get rid of OC_ON_LEAD define. Use secondary linear scorer.
#endif

#ifdef TITLE_VERSION
   #error Get rid of TITLE_VERSION define. Use config task title.
#endif

#ifdef IO_SHOW_HCP_CTRL_SPLIT
   #error Get rid of IO_SHOW_HCP_CTRL_SPLIT define. Use "POSTMORTEM: HCP" in config.
#endif

#ifdef IO_HCP_MIN
   #error Get rid of IO_HCP_MIN define. Use "POSTMORTEM: HCP" in config, then config.postm.minHCP.
#endif

#ifdef IO_HCP_MAX
   #error Get rid of IO_HCP_MAX define. Use "POSTMORTEM: HCP" in config, then config.postm.maxHCP
#endif

#ifdef DEP_SEEK_OPENING_LEAD
   #error Get rid of DEP_SEEK_OPENING_LEAD define. Use "POSTMORTEM: LEAD" in config + TTYPE_SEEK_OPENING_LEAD. 
#endif

#ifdef SOLVE_TWICE_HANDLED_CHUNK
   #error Get rid of SOLVE_TWICE_HANDLED_CHUNK define. Use config.solve.shouldSolveTwice -- autodetected by scorer codes.
#endif

#ifdef SHOW_SACRIFICE_RESULTS
   #error Get rid of SHOW_SACRIFICE_RESULTS define. Use config.solve.seekDecisionCompete -- autodetected by scorer codes. 
#endif

#ifdef SEEK_MAGIC_FLY 
   #error Get rid of SEEK_MAGIC_FLY define. Use config.io.showMagicFly -- autodetected by scorer codes.
#endif

#ifdef FIXED_HAND_NORTH
   #error Get rid of FIXED_HAND_NORTH define. Use config.deck.fixedHand -- autodetected by task type.
#elif defined( FIXED_HAND_WEST )
   #error Get rid of FIXED_HAND_WEST define. Use config.deck.fixedHand -- autodetected by task type.
#endif 

#ifdef SEEK_BIDDING_LEVEL
   #error Get rid of SEEK_BIDDING_LEVEL define. Use (config.solve.taskType == TTYPE_ONE_SIDED_BIDDING_LEVEL) in 
#endif

#ifdef SEM_ONE_SIDED_BIDDING_LEVEL
   #error Get rid of SEM_ONE_SIDED_BIDDING_LEVEL define. Use config.solve.taskType
#endif

#ifdef SEEK_DECISION_COMPETE
   #error Get rid of SEEK_DECISION_COMPETE define. Use config.solve.seekDecisionCompete
#endif

//#ifdef 
//   #error Get rid of  define. Use  in config.
//#endif
 