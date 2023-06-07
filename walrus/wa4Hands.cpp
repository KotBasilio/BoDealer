/************************************************************
* Walrus project                                        2023
* Semantic parts for 4-hand tasks:
* -- how do we init the deck;
* -- how do we iterate.
*
************************************************************/

#include "walrus.h"

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

twPermutedContexts::twPermutedContexts
   (const SplitBits& a, const SplitBits& b, const SplitBits& c)
   : xA(a), xB(b), xC(c)
{
   // copy to form a certain order:
   // A-B-C-A-B-A-C-B-A
   lay[3] = xA;
   lay[5] = xA;
   lay[8] = xA;
   lay[4] = xB;
   lay[7] = xB;
   lay[6] = xC;
}

void Walrus::Permute(SplitBits a, SplitBits b, SplitBits c)
{
   // when a hand D is fixed, we get 6 options to lay A,B,C
   twPermutedContexts xArr(a,b,c);
   ClassifyOnPermute(xArr.lay + 0);// ABC
   ClassifyOnPermute(xArr.lay + 1);// BCA
   ClassifyOnPermute(xArr.lay + 2);// CAB
   ClassifyOnPermute(xArr.lay + 4);// BAC
   ClassifyOnPermute(xArr.lay + 5);// ACB
   ClassifyOnPermute(xArr.lay + 6);// CBA
}

void Walrus::ClassifyOnPermute(twContext* lay)
{
   // run all micro-filters on this 3-hands layout
   // and mark reason why we skip this board
   uint camp = ORDER_BASE;
   for (auto mic: sem.vecFilters) {
      if (auto reason = (filter.*mic.func)(lay, mic.params)) {
         progress.hitsCount[camp][reason]++; 
         return;
      } 
      camp++;
   }

   // save all three hands
   if (mul.countToSolve < mul.maxTasksToSolve) {
      //mul.arrToSolve[mul.countToSolve++].Init(lay); TODO
   }

   // mark together all saved boards
   progress.hitsCount[IO_ROW_THEIRS][0]++;

   // any extra work
   (this->*sem.onBoardAdded)(lay);
}

#ifdef SEMANTIC_SPLINTER_SHAPE
void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onScanCenter = &Walrus::Scan4Hands;
   sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.scanCover = SYMM * 6; // see Permute()
   sem.vecFilters.clear();
   ADD_VOID_FILTER( OK100 );
   ADD_1PAR_FILTER( OKNum, 50 );
   ADD_5PAR_FILTER( ExactShape, NORTH, 4, 4, 4, 1);
   //ADD_VOID_FILTER( RejectAll );
}
#endif // SEMANTIC_SPLINTER_SHAPE

