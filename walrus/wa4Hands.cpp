/************************************************************
* Walrus project                                        2023
* Semantic parts for 4-hand tasks:
* -- how do we init the deck;
* -- how do we iterate.
*
************************************************************/

#include "walrus.h"

#ifdef SEMANTIC_SPLINTER_SHAPE
// Trivial: one hand only; can be used as a pattern for all scans
void Walrus::ScanTricolor()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   for (int idxHandStart = 0;;) {
      // a dummy part, count nothing, just check total
      uint foo = 0;
      uint bar = 0;

      // account the hand
      progress.hitsCount[foo][bar]++;

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      sum.card.jo += shuf.deck[13 + idxHandStart++].card.jo;

      // smart-exit using highBits placed after shuf.deck
      if (sum.IsEndIter()) {
         break;
      }
   }
}

void Walrus::FillSemantic(void)
{
   //sem.onInit = &Walrus::WithdrawByInput;
   sem.fillFlipover = &Walrus::FillFO_MaxDeck;
   sem.onScanCenter = &Walrus::ScanTricolor;
   sem.scanCover = ACTUAL_CARDS_COUNT * 2; // since we flip the hands
   sem.onFilter = &WaFilter::Splinter;
}

uint WaFilter::Splinter(SplitBits& partner, uint& camp, SplitBits& lho, SplitBits& rho)
{
   const uint PASS_BASE = ANSWER_ROW_IDX;
   const uint ORDER_BASE = 3;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   return PASS_BASE;
}

#endif // SEMANTIC_SPLINTER_SHAPE

