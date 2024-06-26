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
      progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_MORE_NT]++;
   } else if (tricksNT == trickSuit) {
      progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_SAME_NT]++;
   } else {
      progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_PREFER_SUIT]++;
   }

   // extra mark for the fly
   progress.countExtraMarks++;
}

void Walrus::NoticeSacrificePossible(uint tOurs, uint tTheirs)
{
   DEBUG_UNEXPECTED;
}

void Walrus::NoticeBidProfit(uint tOurs, uint tTheirs)
{
   // detect score
   CumulativeScore tester;// don't use with adjustable scorers. They are not inited
   (tester.*sem.onScoring)(tOurs);
   (tester.*sem.onSolvedTwice)(tTheirs);

   // mark
   if (tester.bidGame > tester.oppCtrDoubled) {
      progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_PREFER_TO_BID]++;
   } else {
      progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_REFRAIN_BIDDING]++;
   }
   progress.countExtraMarks++;
   s64   delta = tester.bidGame - tester.oppCtrDoubled;
   ui.primaBetterBy += delta;

   // debug
   #ifdef SHOW_EACH_COMPARISON
      char *action = "Bidding   ";
      if (delta < 0) {
         action = "Refraining";
         delta = -delta;
      }
      printf("   %4lld; %4lld. %s is better by %lld points\n", tester.bidGame, tester.oppCtrDoubled, action, delta);
   #endif 
}

void Walrus::CompareSlams(uint tricksA, uint tricksB)
{
   // detect score
   CumulativeScore tester;// don't use with adjustable scorers. They are not inited
   (tester.*sem.onScoring)(tricksA);
   (tester.*sem.onSolvedTwice)(tricksB);

   // mark
   if (tester.ourOther > tester.bidSlam) {
      progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_PREFER_TO_BID]++;
   } else if (tester.ourOther == tester.bidSlam) {
      progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_NO_DIFF]++;
   } else {
      progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_REFRAIN_BIDDING]++;
   }
   progress.countExtraMarks++;

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
      progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_PREFER_PRIMA]++;
   } else if (gainPrima == gainSecunda) {
      progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_NO_DIFF]++;
   } else {
      progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_PREFER_SECUNDA]++;
   }
   progress.countExtraMarks++;

   // add up. may also convert to imps
   auto delta = gainPrima - gainSecunda;
   ui.primaBetterBy += delta;
}

