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
   // implementation of scorers is to change cumulative score
   // so we make a first version to go on with it
   // but it's not great. we'd better rework scorers
   CumulativeScore backup = cumulScore;

   // detect score
   cumulScore.bidGame = 0;
   cumulScore.oppCtrDoubled = 0;
   DdsTricks tester;
   tester.plainScore = tOurs;
   (cumulScore.*sem.onScoring)(tester.plainScore);
   tester.plainScore = tTheirs;
   (this->*sem.onSolvedTwice)(tester);

   // mark
   if (cumulScore.bidGame > cumulScore.oppCtrDoubled) {
      progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_PREFER_TO_BID]++;
   } else {
      progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_REFRAIN_BIDDING]++;
   }
   progress.countExtraMarks++;
   ui.biddingBetterBy = cumulScore.bidGame - cumulScore.oppCtrDoubled;

   // debug
   #ifdef SHOW_EACH_COMPARISON
      char *action = "Bidding   ";
      auto amount = ui.biddingBetterBy;
      if (amount < 0) {
         action = "Refraining";
         amount = -amount;
      }
      printf("   %4lld; %4lld. %s is better by %lld points\n", cumulScore.bidGame, cumulScore.oppCtrDoubled, action, amount);
   #endif 

   // step back
   cumulScore = backup;
}

void Walrus::CountComboScore(uint trickSuit, uint tricksNT)
{
   s64    deltaCombo = 0L;
   if (tricksNT > 8) {
      cumulScore.Opp_3NT(deltaCombo, tricksNT);
   } else if (trickSuit > 10) {
      cumulScore.Opp_5minor(deltaCombo, trickSuit);
   } else {
      uint tricksSet = min(9-tricksNT, 11-trickSuit);
      deltaCombo = 100 * tricksSet;
   }
   cumulScore.ourCombo -= deltaCombo;
}


