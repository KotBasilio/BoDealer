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
   progress.SolvedNormalMark(row, camp);
}

CumulativeScore::Adjustable::Adjustable() 
   : linearBase(nullptr)
   , outSum(nullptr) 
   , title("none")
{}

bool CumulativeScore::Adjustable::IsEmpty() const
{
   return outSum == nullptr;
}

bool CumulativeScore::Adjustable::Init(s64 &out, const char* code)
{
   linearBase = FindLinearScore(code);
   if (linearBase) {
      TargetOut(out);
      title = code;
      return true;
   }

   owl.Show("Failed on encoding scorer for code: %s\n", code);
   return false;
}

void CumulativeScore::Adjustable::TargetOut(s64& out)
{
   outSum = &out;
}

s64 CumulativeScore::Adjustable::Get(uint tricks)
{
   return linearBase[tricks];
}

void CumulativeScore::Adjustable::operator()(uint tricks)
{
   *outSum += Get(tricks);
}

void CumulativeScore::FillSameLinears(const CumulativeScore& other)
{
   prima.  FillUpon(&ideal, other.prima,   &other.ideal);
   secunda.FillUpon(&ideal, other.secunda, &other.ideal);
   tertia. FillUpon(&ideal, other.tertia,  &other.ideal);
}

void CumulativeScore::Adjustable::FillUpon(s64* ourBase, const Adjustable& other, const s64* thatBase)
{
   if (other.IsEmpty()) {
      return;
   }

   // src
   linearBase = other.linearBase;

   // dst
   auto offset = other.outSum - thatBase;
   outSum = ourBase + offset;
}

void CumulativeScore::BiddingLevel(uint tricks)
{
   // find out ideal
   auto gainGame = prima.Get(tricks);
   auto gainPartscore = secunda.Get(tricks);
   ideal += gainGame > gainPartscore ? gainGame : gainPartscore;

   // accumulate regular scores
   prima(tricks);
   secunda(tricks);
}

void CumulativeScore::OpeningLead(DdsTricks &tr)
{
   // accumulate regular score
   prima(tr.plainScore);

   // accumulate results of each lead
   leadS += prima.Get(tr.lead.S);
   leadH += prima.Get(tr.lead.H);
   leadD += prima.Get(tr.lead.D);
   leadC += prima.Get(tr.lead.C);

}
