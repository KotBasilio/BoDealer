/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>

#include "walrus.h"

static void DbgShowComparison(s64 gainPrima, s64 gainSecunda, s64 delta)
{
   if (!config.dbg.verboseComparisons) {
      return;
   }

   printf("  Actions: A: %4lld; B: %4lld. ", gainPrima, gainSecunda);
   const char* action = "A";
   if (delta < 0) {
      action = "B";
      delta = -delta;
   }
   if (delta > 0) {
      printf("%s is better by %lld points\n", action, delta);
   } else {
      printf("Any action is OK\n");
   }
}

void Walrus::NoticeMagicFly(uint trickSuit, uint tricksNT, const deal& cards)
{
   if (tricksNT > trickSuit) {
      progress.SolvedExtraMark(IO_ROW_MAGIC_FLY, IO_CAMP_MORE_NT);
   } else if (tricksNT == trickSuit) {
      progress.SolvedExtraMark(IO_ROW_MAGIC_FLY, IO_CAMP_SAME_NT);
   } else {
      progress.SolvedExtraMark(IO_ROW_MAGIC_FLY, IO_CAMP_PREFER_SUIT);
   }
}

void Walrus::CompareOurContracts(uint tricksA, uint tricksB, const deal& cards)
{
   // detect score
   auto gainPrima   = cumulScore.prima.Get(tricksA);
   auto gainSecunda = cumulScore.secunda.Get(tricksB);

   // mark
   uint camp = (gainPrima >  gainSecunda) ? IO_CAMP_PREFER_PRIMA :
               (gainPrima == gainSecunda) ? IO_CAMP_NO_DIFF : IO_CAMP_PREFER_SECUNDA;
   progress.SolvedExtraMark(IO_ROW_COMPARISON, camp);
   AddMarksByCamp(camp, cards);

   // add up. may also convert to imps
   auto delta = gainPrima - gainSecunda;
   ui.primaBetterBy += delta;

   // dbg
   DbgShowComparison(gainPrima, gainSecunda, delta);
}

//#define DBG_SHOW_EACH_COMPARISON

void Walrus::NoticeBidProfit(uint tOurs, uint tTheirs, const deal& cards)
{
   // detect score
   auto gainPrima   = cumulScore.prima.Get(tOurs);
   auto gainSecunda = - cumulScore.secunda.Get(tTheirs);

   // mark 
   if (gainPrima > gainSecunda) {
      progress.SolvedExtraMark(IO_ROW_SACRIFICE, IO_CAMP_PREFER_TO_BID);
   } else {
      progress.SolvedExtraMark(IO_ROW_SACRIFICE, IO_CAMP_REFRAIN_BIDDING);
   }

   // add up. may also convert to imps
   auto delta = gainPrima - gainSecunda;
   ui.primaBetterBy += delta;

   // debug
#ifdef DBG_SHOW_EACH_COMPARISON
   zzzDbgShowComparison(gainPrima, gainSecunda, delta);
   char *action = "Bidding   ";
   if (delta < 0) {
      action = "Refraining";
      delta = -delta;
   }
   printf("   %4lld; %4lld. %s is better by %lld points\n", gainPrima, gainSecunda, action, delta);
#endif 
}

// looks unused
//#define min(a,b) (((a) < (b)) ? (a) : (b))

