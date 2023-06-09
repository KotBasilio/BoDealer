/************************************************************
* Walrus project                                        2023
* Semantic parts for 4-hand tasks:
* -- how do we init the deck;
* -- how do we iterate.
*
************************************************************/

#include "walrus.h"

#ifndef IO_ROW_SELECTED 
   #define IO_ROW_SELECTED  10
#endif

#ifndef IO_ROW_FILTERING
   #define IO_ROW_FILTERING 3
#endif

void Walrus::Scan4Hands()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   SplitBits third(Sum3rdHand());
   SplitBits stop(sum, sec, third);
   for (int idxHandStart = 0; third != stop;) {
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
   }
}

twPermutedContexts::twPermutedContexts
   (const SplitBits& a, const SplitBits& b, const SplitBits& c)
   : xA(a), xB(b), xC(c)
{
   // copy to form a certain order:
   // A-B-C-A-B-A-C-B-A-D
   // 0 1 2 3 4 5 6 7 8 9
   lay[ 3] = xA;
   lay[ 4] = xB;
   lay[ 5] = xA;
   lay[ 6] = xC;
   lay[ 7] = xB;
   lay[ 8] = xA;
   lay[ 9] = twContext( SplitBits(a, b, c) );
}

void Walrus::Permute(SplitBits a, SplitBits b, SplitBits c)
{
   // when a hand D is fixed in the end, we get 6 options to lay A,B,C
   twPermutedContexts xArr(a,b,c);
   ClassifyAndPull  (xArr.lay + 6);// CBAD
   ClassifyAndPull  (xArr.lay + 5);// ACBD
   ClassifyOnPermute(xArr.lay + 4);// BACD
   xArr.lay[5] = xArr.lay[9];      // pull D with a skip
   ClassifyAndPull  (xArr.lay + 2);// CABD
   ClassifyAndPull  (xArr.lay + 1);// BCAD
   ClassifyAndPull  (xArr.lay + 0);// ABCD
}

void Walrus::ClassifyAndPull(twContext* lay) 
{
   // we pull-back the D hand with each call
   ClassifyOnPermute(lay);
   lay[SOUTH] = lay[WEST];
}

void Walrus::ClassifyOnPermute(twContext* lay)
{
   // run all micro-filters on this 3-hands layout
   // and mark reason why we skip this board
   uint camp = IO_ROW_FILTERING;
   for (const auto &mic: sem.vecFilters) {
      if (auto reason = (filter.*mic.func)(lay, mic.params)) {
         progress.hitsCount[camp][reason]++; 
         return;
      } 
      camp++;
   }

   // save all three hands
   if (mul.countToSolve < mul.maxTasksToSolve) {
      mul.arrToSolve[mul.countToSolve++].Init(lay);
   }

   // mark together all saved boards
   progress.hitsCount[IO_ROW_SELECTED][0]++;

   // any extra work
   (this->*sem.onBoardAdded)(lay);
}

void DdsTask3::Init(twContext* lay) 
{ 
   north = lay[NORTH].hand; 
   east  = lay[EAST].hand; 
   south = lay[SOUTH].hand; 
}

#ifdef SEMANTIC_SPLINTER_SHAPE
void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.onScanCenter = &Walrus::Scan4Hands;
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   //sem.onBoardAdded = &Walrus::GrabSplinterVariant;
   sem.onScoring = &Walrus::Score_NV6Major;
   //sem.onAfterMath = &Walrus::SolveSavedTasks;
   sem.scanCover = SYMM * 6; // see Permute()
   sem.vecFilters.clear();
   ADD_4PAR_FILTER( NORTH, ExactShape, 4, 4, 4, 1);
   ADD_2PAR_FILTER( SOUTH, SpadesLen, 5, 6);
   ADD_2PAR_FILTER( SOUTH, PointsRange, 11, 12);
   ADD_1PAR_FILTER( NORTH, PointsAtLeast, 10);
   ADD_3PAR_FILTER( NORTH, LineControlsRange, SOUTH, 9, 9);
   ADD_1PAR_FILTER( NORTH, ClubPointsLimit, 1);
   ADD_0PAR_FILTER( WEST,  NoOvercall );
   ADD_0PAR_FILTER( SOUTH, SpadesNatural );
   ADD_0PAR_FILTER( EAST,  No7Plus );
   ADD_0PAR_FILTER( WEST,  No2SuiterAntiSpade );

   // used previously
   // ADD_2PAR_FILTER( SOUTH, ControlsRange, 4, 10);
   // ADD_2PAR_FILTER( NORTH, ControlsRange, 4, 10);
}

void Walrus::GrabSplinterVariant(twContext* lay)
{
   const auto &hcp_N(lay[NORTH].hcp);
   const auto &len_S(lay[SOUTH].len);
   const auto &ctr_S(lay[SOUTH].ctrl);

   // having the Ac
   progress.hitsCount[hcp_N.total][(ctr_S.c > 1) ? 1 : 3]++;

   // length of clubs
   progress.hitsCount[0][len_S.c]++;

   // balance
   progress.countExtraMarks += 2;
}
#endif // SEMANTIC_SPLINTER_SHAPE

