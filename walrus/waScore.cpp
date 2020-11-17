/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>

#include "walrus.h"
#include "..\dds-develop\include\dll.h"
#include "..\dds-develop\examples\hands.h"

#define OUR_ROW   0
#define THEIR_ROW 10

void Walrus::HitByScore(DdsTricks &tr, uint made, uint row /*= OUR_ROW*/)
{
   uint camp = 0;
   if (tr.plainScore >= made) {
      row++;
      camp = tr.plainScore - made;
   } else {
      camp = made - tr.plainScore - 1;
   }
   hitsCount[row][camp]++;
}


void Walrus::Score_4Major(DdsTricks& tr)
{
   // hits
   HitByScore(tr, 10);

   // cumul
   Score_Cumul4M(tr);
}

void Walrus::Score_NV_4Major(DdsTricks& tr)
{
   // hits
   HitByScore(tr, 10);

   // cumul
   Score_CumulNV4M(tr);
}

void Walrus::Score_3NT(DdsTricks& tr)
{
   // hits
   HitByScore(tr, 9);

   // cumul
   Score_Cumul3NT(tr);
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

void Walrus::Score_Opp3MajorDoubled(DdsTricks& tr)
{
   // we don't need any hits; need only a cumulative score
   cumulScore.Opp_3Major(cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_3MajX(cumulScore.oppCtrDoubled, tr.plainScore);
}

void Walrus::Score_Opp4MajorDoubled(DdsTricks& tr)
{
   // store their hits
   HitByScore(tr, 10, THEIR_ROW);
   countOppContractMarks++;

   // store their cumulative score
   cumulScore.Opp_4MajX(cumulScore.oppCtrDoubled, tr.plainScore);
}

void Walrus::Score_NV6Major(DdsTricks& tr)
{
   // hits
   HitByScore(tr, 12);

   // cumul
   Score_CumulNV6M(tr);
}

void Walrus::Score_NV6Minor(DdsTricks& tr)
{
   // hits
   HitByScore(tr, 12);

   // cumul
   cumulScore.Our6mNV(tr.plainScore);
}
