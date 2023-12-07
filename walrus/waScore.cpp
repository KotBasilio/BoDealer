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

void Walrus::HitByScore(DdsTricks &tr, uint made, uint row /*= IO_ROW_OUR_DOWN*/)
{
   uint camp = 0;
   if (tr.plainScore >= made) {
      row++;
      camp = tr.plainScore - made;
   } else {
      camp = made - tr.plainScore - 1;
   }
   progress.hitsCount[row][camp]++;
}

void Walrus::Score_4Major(DdsTricks& tr)
{
   cumulScore.Our4M(tr.plainScore);
}

void Walrus::Score_NV_4Major(DdsTricks& tr)
{
   cumulScore.OurNV4M(tr.plainScore);
}

void Walrus::Score_NV_Doubled4Major(DdsTricks &tr)
{
   cumulScore.OurNV4MX(tr.plainScore);
}

void Walrus::Score_5Major(DdsTricks &tr)
{
   cumulScore.Our5M(tr.plainScore);
}

void Walrus::Score_NV_5Major(DdsTricks &tr)
{
   cumulScore.OurNV5M(tr.plainScore);
}

void Walrus::Score_4Minor(DdsTricks& tr)
{
   cumulScore.Our4minor(tr.plainScore);
}

void Walrus::Score_5Minor(DdsTricks& tr)
{
   cumulScore.Our5minor(tr.plainScore);
}

void Walrus::Score_NV_5Minor(DdsTricks &tr)
{
   cumulScore.OurNV5minor(tr.plainScore);
}

void Walrus::Score_NV_Doubled5Minor(DdsTricks& tr)
{
   cumulScore.OurNV5mX(tr.plainScore);
}

void Walrus::Score_3NT(DdsTricks& tr)
{
   cumulScore.Our3NT(tr.plainScore);
}

void Walrus::NoticeMagicFly(uint trickSuit, uint tricksNT)
{
   if (tricksNT > trickSuit) {
      progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_MORE_NT]++;
   }
   else if (tricksNT == trickSuit) {
      progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_SAME_NT]++;
   }
   else {
      progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_PREFER_SUIT]++;
   }

   // extra mark for the fly
   progress.countExtraMarks++;
}

void Walrus::NoticeSacrificePossible(uint trickSuit, uint tricksNT)
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
   (this->*sem.onScoring)(tester);
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

void Walrus::Score_Doubled3NT(DdsTricks &tr)
{
#ifdef SEEK_DECISION_WHETHER_DOUBLE
   cumulScore.Opp_3NT (cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_3NTX(cumulScore.oppCtrDoubled, tr.plainScore);
#endif // SEEK_DECISION_WHETHER_DOUBLE
}

void Walrus::Score_2m(DdsTricks &tr)
{
   DEBUG_UNEXPECTED;
}

void Walrus::Score_Opp3Major(DdsTricks& tr)
{
   // both contracts
   cumulScore.Opp_3Major(cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_3MajX(cumulScore.oppCtrDoubled, tr.plainScore);
}

void Walrus::Score_Opp5MinorDoubled(DdsTricks& tr)
{
   // both contracts
   cumulScore.Opp_5minor(cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_5minorX(cumulScore.oppCtrDoubled, tr.plainScore);
}

void Walrus::Score_Opp4Major(DdsTricks &tr)
{
   // both contracts
   cumulScore.Opp_4M(cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_4MajX(cumulScore.oppCtrDoubled, tr.plainScore);
}

void Walrus::Score_Opp3NT(DdsTricks& tr)
{
   cumulScore.Opp_3NT(cumulScore.oppContract, tr.plainScore);
   cumulScore.ourOther = -cumulScore.oppContract;
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

void Walrus::Score_NV6Major(DdsTricks& tr)
{
   cumulScore.OurNV6Maj(tr.plainScore);
}

void Walrus::Score_NV6Minor(DdsTricks& tr)
{
   cumulScore.OurNV6m(tr.plainScore);
}

void Walrus::Score_NV7Minor(DdsTricks& tr)
{
   cumulScore.OurNV7m(tr.plainScore);
}

void Walrus::Score_NV6NoTrump(DdsTricks &tr)
{
   cumulScore.OurNV6_No(tr.plainScore);
}

void Walrus::Score_6NT(DdsTricks &tr)
{
   cumulScore.Our6_No(tr.plainScore);
}


