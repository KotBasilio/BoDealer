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
   CumulativeScore tester;
   (tester.*sem.onScoring)(tOurs);
   (tester.*sem.onSolvedTwice)(tTheirs);

   // mark
   if (tester.bidGame > tester.oppCtrDoubled) {
      progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_PREFER_TO_BID]++;
   } else {
      progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_REFRAIN_BIDDING]++;
   }
   progress.countExtraMarks++;
   ui.biddingBetterBy = tester.bidGame - tester.oppCtrDoubled;

   // debug
   #ifdef SHOW_EACH_COMPARISON
      char *action = "Bidding   ";
      auto amount = ui.biddingBetterBy;
      if (amount < 0) {
         action = "Refraining";
         amount = -amount;
      }
      printf("   %4lld; %4lld. %s is better by %lld points\n", tester.bidGame, tester.oppCtrDoubled, action, amount);
   #endif 
}

void Walrus::CompareOurContracts(uint tricksA, uint tricksB)
{
}

// OBSOLETE
// void Walrus::CountComboScore(uint trickSuit, uint tricksNT)
// {
//    s64    deltaCombo = 0L;
//    if (tricksNT > 8) {
//       cumulScore.Opp_3NT(deltaCombo, tricksNT);
//    } else if (trickSuit > 10) {
//       cumulScore.Opp_5minor(deltaCombo, trickSuit);
//    } else {
//       uint tricksSet = min(9-tricksNT, 11-trickSuit);
//       deltaCombo = 100 * tricksSet;
//    }
//    cumulScore.ourCombo -= deltaCombo;
// ...
//    owl.OnDone("Combo-score average for our two contracts = %lld.\n", cumulScore.ourCombo / sumOppRows);
// }


