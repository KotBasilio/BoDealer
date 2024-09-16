/************************************************************
* Walrus project                                        2023
* Semantic parts for 4-hand tasks:
* -- how do we init the deck;
* -- how do we iterate.
*
************************************************************/

#include "walrus.h"

#ifndef IO_ROW_FILTERING
   #define IO_ROW_FILTERING 3
#endif

#define PERMUTE_24
#ifdef PERMUTE_24
   #define PERMUTE_FACTOR 24
   #define Permute  Permute24
#else
   #define PERMUTE_FACTOR 6
   #define Permute  Permute6
#endif // PERMUTE_24

void Walrus::Scan4Hands()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   SplitBits third(Sum3rdHand());
   SplitBits stop(sum, sec, third);
   for (int idxHandStart = 0; third != stop;) {
      // do all permutation of 4 hands
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

#define FULL_PART_0  0
#define FULL_PART_1  (FULL_PART_0 + SIZE_PERMUTE_PATTERN - 3)
#define FULL_PART_2  (FULL_PART_1 + SIZE_PERMUTE_PATTERN - 2)
#define FULL_PART_3  (FULL_PART_2 + SIZE_PERMUTE_PATTERN - 1)

void Walrus::Permute24(SplitBits a, SplitBits b, SplitBits c)
{
   twPermutedFullFlip xFull(a,b,c);
   Classify6(xFull.lay + FULL_PART_3);
   Classify6(xFull.lay + FULL_PART_2);
   Classify6(xFull.lay + FULL_PART_1);
   Classify6(xFull.lay + FULL_PART_0);
}

twPermutedFullFlip::twPermutedFullFlip
   (const SplitBits& a, const SplitBits& b, const SplitBits& c)
   : p6(a, b, c)
{
   // first section is in place, ending with (B-A-D). we reuse 3
   LayPattern(FULL_PART_1, 2);

   // second section is in place, ending with (A-B-C). we reuse 2
   lay[FULL_PART_2 + 2] = lay[9];
   LayPattern(FULL_PART_2, 0);

   // third section is in place, ending with (C-B-A). we reuse 1
   lay[FULL_PART_3 + 1] = lay[9];
   lay[FULL_PART_3 + 2] = p6.xC;
   LayPattern(FULL_PART_3, 1);

   // all sections are ready, ending with (D-A-B)
}

void twPermutedFullFlip::LayPattern(uint dest, uint iNewD)
{
   // address contexts
   const twContext& xA(lay[dest + 0]);
   const twContext& xB(lay[dest + 1]);
   const twContext& xC(lay[dest + 2]);
   const twContext& xD(lay[iNewD]);

   // (A,B,C) is in place
   // copy to form a the same order:
   // A-B-C-A-B-A-C-B-A-D
   // 0 1 2 3 4 5 6 7 8 9
   lay[dest + 3] = xA;
   lay[dest + 4] = xB;
   lay[dest + 5] = xA;
   lay[dest + 6] = xC;
   lay[dest + 7] = xB;
   lay[dest + 8] = xA;
   lay[dest + 9] = xD;
}

void Walrus::ClassifyAndPull(twContext* lay) 
{
   // we pull-back the D hand with each call
   ClassifyOnPermute(lay);
   lay[SOUTH] = lay[WEST];
}

void Walrus::ClassifyAndPush(twContext* lay) 
{
   // we push forward the A hand with each call
   ClassifyOnPermute(lay);
   lay[EAST] = lay[NORTH];
}

void Walrus::ClassifyOnPermute(twContext* lay)
{
   // board out => goodbye
   if (filter.ScanOut(lay)) {
      return;
   }

   // usually add the board
   (this->*sem.onBoardFound)(lay);
}

void Walrus::AddForSolving(twContext* lay)
{
   // save all three hands
   mul.SaveThreeHands(lay);

   // mark together all saved boards
   progress.SelectedMark();

   // any extra work
   (this->ui.*sem.onBoardAdded)(lay);
}

void WaMulti::SaveThreeHands(twContext* lay)
{
   if (countToSolve < maxTasksToSolve) {
      arrToSolve[countToSolve++].Init(lay);
   } else {
      shouldSignOut = true;
   }
}

void WaTask3::Init(twContext* lay) 
{ 
   north = lay[NORTH].hand; 
   east  = lay[EAST].hand; 
   south = lay[SOUTH].hand; 
}

void WaTask2::Init(twContext* lay) 
{ 
   partner = lay[SOUTH].hand; 
   oneOpp  = lay[EAST].hand; 
}

void Walrus::FourHandsFillSem(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.onScanCenter = &Walrus::Scan4Hands;
   sem.onAfterMath = &Walrus::SolveSavedTasks;
   sem.scanCover = SYMM * PERMUTE_FACTOR;
}

