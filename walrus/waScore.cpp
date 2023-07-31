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
   // hits
   HitByScore(tr, 10);

   // cumul
   cumulScore.Our4M(tr.plainScore);
}

void Walrus::Score_NV_4Major(DdsTricks& tr)
{
   HitByScore(tr, 10);
   cumulScore.OurNV4M(tr.plainScore);
}

void Walrus::Score_NV_Doubled4Major(DdsTricks &tr)
{
   HitByScore(tr, 10);
   cumulScore.OurNV4MX(tr.plainScore);
}

void Walrus::Score_5Major(DdsTricks &tr)
{
   HitByScore(tr, 11);
   cumulScore.Our5M(tr.plainScore);
}

void Walrus::Score_NV_5Major(DdsTricks &tr)
{
   HitByScore(tr, 11);
   cumulScore.OurNV5M(tr.plainScore);
}

void Walrus::Score_5Minor(DdsTricks& tr)
{
   HitByScore(tr, 11);
   cumulScore.Our5minor(tr.plainScore);
}

void Walrus::Score_NV_5Minor(DdsTricks &tr)
{
   HitByScore(tr, 11);
   cumulScore.OurNV5minor(tr.plainScore);
}

void Walrus::Score_NV_Doubled5Minor(DdsTricks& tr)
{
   HitByScore(tr, 11);
   cumulScore.OurNV5mX(tr.plainScore);
}

void Walrus::Score_3NT(DdsTricks& tr)
{
   // hits
   HitByScore(tr, 9);

   // cumul
   cumulScore.Our3NT(tr.plainScore);
}

void Walrus::Score_MagicFly(DdsTricks& tr)
{
   HitByScore(tr, 9, IO_ROW_THEIRS);
}

void Walrus::NoticeMagicFly(uint trickSuit, uint tricksNT)
{
   if (tricksNT > trickSuit) {
      progress.hitsCount[IO_ROW_MYFLY][IO_CAMP_MORE_NT]++;
   }
   else if (tricksNT == trickSuit) {
      progress.hitsCount[IO_ROW_MYFLY][IO_CAMP_SAME_NT]++;
   }
   else {
      progress.hitsCount[IO_ROW_MYFLY][IO_CAMP_PREFER_SUIT]++;
   }

   // extra marks: one for 3NT result, and one for the fly
   progress.countExtraMarks += 2;
}

void Walrus::Score_Doubled3NT(DdsTricks &tr)
{
   // hits
   HitByScore(tr, 9);

#ifdef SEEK_DECISION_OVER_DOUBLE
   cumulScore.Opp_3NT (cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_3NTX(cumulScore.oppCtrDoubled, tr.plainScore);
#endif // SEEK_DECISION_OVER_DOUBLE
}

void Walrus::Score_2m(DdsTricks &tr)
{
   // hits
   HitByScore(tr, 8);

#ifdef SHOW_OPP_RESULTS
   cumulScore.Opp_2m(cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_2mX(cumulScore.oppCtrDoubled, tr.plainScore);
#endif 
}

void Walrus::Score_Opp3MajorDoubled(DdsTricks& tr)
{
   // store their hits
   HitByScore(tr, 9, IO_ROW_THEIRS);
   progress.countExtraMarks++;

   // cumulative score
   cumulScore.Opp_3Major(cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_3MajX(cumulScore.oppCtrDoubled, tr.plainScore);
}

void Walrus::Score_Opp3Major(DdsTricks& tr)
{
   // this scores both
   Score_Opp3MajorDoubled(tr);
}

void Walrus::Score_Opp4MajorDoubled(DdsTricks& tr)
{
   // store their hits
   HitByScore(tr, 10, IO_ROW_THEIRS);
   progress.countExtraMarks++;

   // store their cumulative score
   cumulScore.Opp_NV_4MajX(cumulScore.oppCtrDoubled, tr.plainScore);
}

void Walrus::Score_Opp4Major(DdsTricks &tr)
{
   HitByScore(tr, 10, IO_ROW_THEIRS);
   progress.countExtraMarks++;
   cumulScore.Opp_4M(cumulScore.oppContract, tr.plainScore);
}

void Walrus::Score_Opp3NT(DdsTricks& tr)
{
   HitByScore(tr, 9, IO_ROW_THEIRS);
   progress.countExtraMarks++;
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
   HitByScore(tr, 12);
   cumulScore.OurNV6Maj(tr.plainScore);
}

void Walrus::Score_NV6Minor(DdsTricks& tr)
{
   HitByScore(tr, 12);
   cumulScore.OurNV6m(tr.plainScore);
}

void Walrus::Score_NV6NoTrump(DdsTricks &tr)
{
   HitByScore(tr, 12);
   cumulScore.OurNV6_No(tr.plainScore);
}

void Walrus::Score_6NT(DdsTricks &tr)
{
   HitByScore(tr, 12);
   cumulScore.Our6_No(tr.plainScore);
}

void Walrus::PostmortemHCP(DdsTricks& tr, deal& cards)
{
   // ensure we include this in stat
   uint row = 0;
   uint ctrl;
   auto hcp = WaCalcHCP(cards, ctrl);
   if (ui.minControls) {
      if (ctrl < (uint)ui.minControls) {
         row = IO_ROW_HCP_START;
      } else {
         row = IO_ROW_HCP_START + (ctrl - ui.minControls) * 2;
      }
   } else if (hcp < IO_HCP_MIN || IO_HCP_MAX < hcp) {
      return;
   } else {
      row = IO_ROW_HCP_START + (hcp - IO_HCP_MIN) * 2;
   }

   if (row < IO_ROW_FILTERING - 1) {
      HitByScore(tr, ui.irBase, row);
      progress.countExtraMarks++;
   }
}


