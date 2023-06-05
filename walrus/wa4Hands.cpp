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
   //sem.onInit = &Walrus::WithdrawByInput;
   sem.fillFlipover = &Walrus::FillFO_MaxDeck;
   sem.onScanCenter = &Walrus::ScanTricolor;
   sem.scanCover = 13 * 6; // see permute
   sem.onFilter = &WaFilter::Splinter;
}

void Walrus::ScanTricolor()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   SplitBits third(Sum3rdHand());
   SplitBits stop(shuf.checkSum - sum.card.jo - sec.card.jo - third.card.jo);
   for (int idxHandStart = 0;;) {
      // account the deal several times
      Permute(sum, sec, third);

      // advance to next hands
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      u64 flipcd = shuf.deck[13 + idxHandStart].card.jo;
      sum.card.jo += flipcd;
      sec.card.jo -= flipcd;
      flipcd = shuf.deck[26 + idxHandStart].card.jo;
      sec.card.jo += flipcd;
      third.card.jo -= flipcd;
      flipcd = shuf.deck[39 + idxHandStart++].card.jo;
      third.card.jo += flipcd;

      // smart-exit when last hand meets known
      if (third.card.jo == stop.card.jo) {
         break;
      }
   }
}

void Walrus::Permute(SplitBits a, SplitBits b, SplitBits c)
{
   uint foo = 0, bar = 0;
   foo = (filter.*sem.onFilter)(a, bar, b, c);
   progress.hitsCount[foo][bar]++;
   foo = (filter.*sem.onFilter)(a, bar, c, b);
   progress.hitsCount[foo][bar]++;
   foo = (filter.*sem.onFilter)(b, bar, a, c);
   progress.hitsCount[foo][bar]++;
   foo = (filter.*sem.onFilter)(b, bar, c, a);
   progress.hitsCount[foo][bar]++;
   foo = (filter.*sem.onFilter)(c, bar, a, b);
   progress.hitsCount[foo][bar]++;
   foo = (filter.*sem.onFilter)(c, bar, b, a);
   progress.hitsCount[foo][bar]++;
}

uint WaFilter::Splinter(SplitBits& opener, uint& camp, SplitBits& direct, SplitBits& resp)
{
   const uint PASS_BASE = ANSWER_ROW_IDX;
   const uint ORDER_BASE = 3;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   return PASS_BASE;
}

#endif // SEMANTIC_SPLINTER_SHAPE

