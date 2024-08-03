/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>

#include "walrus.h"
#include "../dds-develop/include/dll.h"
#include "../dds-develop/examples/hands.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

void Walrus::NoticeMagicFly(uint trickSuit, uint tricksNT)
{
   if (tricksNT > trickSuit) {
      progress.ExtraMark(IO_ROW_MAGIC_FLY, IO_CAMP_MORE_NT);
   } else if (tricksNT == trickSuit) {
      progress.ExtraMark(IO_ROW_MAGIC_FLY, IO_CAMP_SAME_NT);
   } else {
      progress.ExtraMark(IO_ROW_MAGIC_FLY, IO_CAMP_PREFER_SUIT);
   }
}

void Walrus::CompareSlams(uint tricksA, uint tricksB)
{
   // detect score -- TODO see CompareOurContracts / NoticeBidProfit
   CumulativeScore tester;
   tester.FillSameLinears(cumulScore);
   (tester.*sem.onDepPrimaryScoring)(tricksA);
   (tester.*sem.onDepSecondScoring)(tricksB);

   // mark
   if (tester.ourOther > tester.bidSlam) {
      progress.ExtraMark(IO_ROW_COMPARISON,IO_CAMP_PREFER_TO_BID);
   } else if (tester.ourOther == tester.bidSlam) {
      progress.ExtraMark(IO_ROW_SACRIFICE,IO_CAMP_NO_DIFF);
   } else {
      progress.ExtraMark(IO_ROW_COMPARISON,IO_CAMP_REFRAIN_BIDDING);
   }

   // add up. may also convert to imps
   auto delta = tester.ourOther - tester.bidSlam;
   ui.primaBetterBy += delta;
}

void Walrus::CompareOurContracts(uint tricksA, uint tricksB)
{
   // detect score
   auto gainPrima   = cumulScore.prima.Get(tricksA);
   auto gainSecunda = cumulScore.secunda.Get(tricksB);

   // mark
   if (gainPrima > gainSecunda) {
      progress.ExtraMark(IO_ROW_COMPARISON,IO_CAMP_PREFER_PRIMA);
   } else if (gainPrima == gainSecunda) {
      progress.ExtraMark(IO_ROW_SACRIFICE,IO_CAMP_NO_DIFF);
   } else {
      progress.ExtraMark(IO_ROW_COMPARISON,IO_CAMP_PREFER_SECUNDA);
   }

   // add up. may also convert to imps
   auto delta = gainPrima - gainSecunda;
   ui.primaBetterBy += delta;
}

//#define DBG_SHOW_EACH_COMPARISON

void Walrus::NoticeBidProfit(uint tOurs, uint tTheirs)
{
   // detect score
   auto gainPrima   = cumulScore.prima.Get(tOurs);
   auto gainSecunda = - cumulScore.secunda.Get(tTheirs);

   // mark 
   if (gainPrima > gainSecunda) {
      progress.ExtraMark(IO_ROW_SACRIFICE, IO_CAMP_PREFER_TO_BID);
   } else {
      progress.ExtraMark(IO_ROW_SACRIFICE, IO_CAMP_REFRAIN_BIDDING);
   }

   // add up. may also convert to imps
   auto delta = gainPrima - gainSecunda;
   ui.primaBetterBy += delta;

   // debug
#ifdef DBG_SHOW_EACH_COMPARISON
   char *action = "Bidding   ";
   if (delta < 0) {
      action = "Refraining";
      delta = -delta;
   }
   printf("   %4lld; %4lld. %s is better by %lld points\n", gainPrima, gainSecunda, action, delta);
#endif 
}

