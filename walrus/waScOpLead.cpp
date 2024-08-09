/************************************************************
 * Walrus project                                        2020
 * Deprecatep parts and diagnostics
 *
 ************************************************************/
#include "walrus.h"

#ifdef INPUT_TRUMPS
   #error Get rid of INPUT_TRUMPS define. Use primary linear scorer.
#endif
#ifdef INPUT_ON_LEAD
   #error Get rid of INPUT_ON_LEAD define. Use primary linear scorer.
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

#ifdef SEEK_OPENING_LEAD
   #error Get rid of SEEK_OPENING_LEAD define. Use "POSTMORTEM: LEAD" in config.
#endif
