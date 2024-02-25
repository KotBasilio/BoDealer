/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>

#include "walrus.h"
#include "../dds-develop/include/dll.h"
#include "../dds-develop/examples/hands.h"

void Walrus::HitByTricks(DdsTricks &tr, uint made, uint row /*= IO_ROW_OUR_DOWN*/)
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

void Walrus::Score_Doubled3NT(DdsTricks &tr)
{
   cumulScore.Opp_3NT (cumulScore.oppContract, tr.plainScore);
   cumulScore.Opp_3NTX(cumulScore.oppCtrDoubled, tr.plainScore);
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

void Walrus::Score_NV7NT(DdsTricks& tr)
{
   cumulScore.OurNV7NT(tr.plainScore);
}
