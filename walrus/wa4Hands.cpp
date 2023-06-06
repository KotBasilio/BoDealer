/************************************************************
* Walrus project                                        2023
* Semantic parts for 4-hand tasks:
* -- how do we init the deck;
* -- how do we iterate.
*
************************************************************/

#include "walrus.h"

#ifdef SEMANTIC_SPLINTER_SHAPE
void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onScanCenter = &Walrus::Scan4Hands;
   sem.scanCover = SYMM * 6; // see permute
   sem.onFilter = &WaFilter::ClassifyOnScan;
   sem.onDepFilter = &WaFilter::Splinter;
}

void Walrus::Scan4Hands()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   SplitBits third(Sum3rdHand());
   SplitBits stop(shuf.CheckSum() - sum.card.jo - sec.card.jo - third.card.jo);
   for (int idxHandStart = 0;;) {
      // account the deal several times
      Permute(sum, sec, third);

      // advance to next hands
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      u64 flipcd = shuf.deck[SYMM + idxHandStart].card.jo;
      sum.card.jo += flipcd;
      sec.card.jo -= flipcd;
      flipcd = shuf.deck[SYMM2 + idxHandStart].card.jo;
      sec.card.jo += flipcd;
      third.card.jo -= flipcd;
      flipcd = shuf.deck[SYMM3 + idxHandStart++].card.jo;
      third.card.jo += flipcd;

      // smart-exit when last hand meets known
      if (third.card.jo == stop.card.jo) {
         break;
      }
   }
}

void Walrus::Permute(SplitBits a, SplitBits b, SplitBits c)
{
   // when a hand D is fixed, we get 6 options to lay A,B,C
   (filter.*sem.onFilter)(a, b, c);
   (filter.*sem.onFilter)(a, c, b);
   (filter.*sem.onFilter)(b, a, c);
   (filter.*sem.onFilter)(b, c, a);
   (filter.*sem.onFilter)(c, a, b);
   (filter.*sem.onFilter)(c, b, a);
}

void WaFilter::ClassifyOnScan(SplitBits a, SplitBits b, SplitBits c)
{
   uint camp = 0;
   uint reason = Splinter(a, camp, b, c);
   if (reason) {
      // there's some reason to skip this hand. mark it
      progress->hitsCount[reason][camp]++;
   } else {
      // save all three hands
      // if (mul.countToSolve < mul.maxTasksToSolve) {
      //    //mul.arrToSolve[mul.countToSolve++].Init(partner, rho);
      // }

      // mark all saved together
      progress->hitsCount[1][1]++;
   }
}

uint WaFilter::Splinter(SplitBits& opener, uint& camp, SplitBits& direct, SplitBits& resp)
{
   const uint ORDER_BASE = 3;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   // seems it passes
   return 0;
}

#endif // SEMANTIC_SPLINTER_SHAPE

