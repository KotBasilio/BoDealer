/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SEM_JUN24_LEAD_AK
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();

   sem.SetOpeningLeadScorer  (cumulScore, "V4H ");
   config.detailedReportType = WREPORT_OPENING_LEADS;
   //sem.onPostmortem = &Walrus::PostmortemHCP;

   //ADD_2PAR_FILTER(SOUTH, PointsSuitLimit, CLB, 0);

}
#endif 
