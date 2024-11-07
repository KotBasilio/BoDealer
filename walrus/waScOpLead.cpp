/************************************************************
 * Walrus project                                        2020
 * Deprecated parts and diagnostics
 *
 ************************************************************/
#include "walrus.h"

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
   #error Get rid of DEP_SEEK_OPENING_LEAD define. Use "POSTMORTEM: LEAD" in config + SEEK_OPENING_LEAD define. 
#endif

#ifdef SOLVE_TWICE_HANDLED_CHUNK
   #error Get rid of SOLVE_TWICE_HANDLED_CHUNK define. Use config.solve.shouldSolveTwice
#endif

#ifdef SHOW_SACRIFICE_RESULTS
   #error Get rid of SHOW_SACRIFICE_RESULTS define. Use config.io.seekDecisionCompete
#endif

//#ifdef 
//   #error Get rid of  define. Use  in config.
//#endif
 