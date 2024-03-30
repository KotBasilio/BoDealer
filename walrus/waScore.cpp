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

void CumulativeScore::Score_Opp3Major(uint tricks)
{
   // both contracts
   Opp_3Major(oppContract,   tricks);
   Opp_3MajX (oppCtrDoubled, tricks);
}

void CumulativeScore::Score_Opp5Minor(uint tricks)
{
   // both contracts
   Opp_5minor (oppContract,   tricks);
   Opp_5minorX(oppCtrDoubled, tricks);
}

void CumulativeScore::Score_Opp4Major(uint tricks)
{
   // both contracts
   Opp_4M   (oppContract,   tricks);
   Opp_4MajX(oppCtrDoubled, tricks);
}

void CumulativeScore::Score_Opp3NT(uint tricks)
{
   Opp_3NT (oppContract,   tricks);
   Opp_3NTX(oppCtrDoubled, tricks);
   ourOther = -oppContract;
}

void CumulativeScore::Our2_NV7NT(uint tricks)
{
   OurNV7NT(ourOther, tricks);
}

void CumulativeScore::OurNV7NT(s64& sum, uint tricks)
{
   // "always slam" strategy
   switch (tricks) {
      case 2:  sum -= 500; break;
      case 3:  sum -= 450; break;
      case 4:  sum -= 350; break;
      case 5:  sum -= 400; break;
      case 6:  sum -= 350; break;
      case 7:  sum -= 300; break;
      case 8:  sum -= 250; break;
      case 9:  sum -= 200; break;
      case 10: sum -= 150; break;
      case 11: sum -= 100; break;
      case 12: sum -= 50;  break;
      case 13: sum += 1520; break;
   }
}

CumulativeScore::Adjustable::Adjustable() 
   : linearBase(nullptr)
   , outSum(nullptr) 
{}

bool CumulativeScore::Adjustable::IsEmpty()
{
   return outSum == nullptr;
}

bool CumulativeScore::Adjustable::Init(s64 &out, const char* code)
{
   linearBase = FindLinearScore(code);
   if (linearBase) {
      outSum = &out;
      return true;
   }

   owl.Show("Failed on encoding scorer for code: %s\n", code);
   return false;
}

s64 CumulativeScore::Adjustable::Get(uint tricks)
{
   return linearBase[tricks];
}

void CumulativeScore::Adjustable::operator()(uint tricks)
{
   *outSum += Get(tricks);
}
