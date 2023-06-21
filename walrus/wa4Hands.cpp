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

// a chunk for watching permutations in debugger
// xA.hand.card.jo = 0xaaaaaaaaaaaaaaaaLL;
// xB.hand.card.jo = 0xbbbbbbbbbbbbbbbbLL;
// xC.hand.card.jo = 0xccccccccccccccccLL;

twPermutedContexts::twPermutedContexts
   (const SplitBits& a, const SplitBits& b, const SplitBits& c)
   : xA(a), xB(b), xC(c)
{
   // (A,B,C) is in place
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

void Walrus::Permute6(SplitBits a, SplitBits b, SplitBits c)
{
   twPermutedContexts xArr(a,b,c);
   Classify6(xArr.lay);
}

void Walrus::Classify6(twContext *lay)
{
   // when a hand D is fixed in the end, we get 6 options to lay A,B,C
   ClassifyAndPull  (lay + 6);// CBAD
   ClassifyAndPull  (lay + 5);// ACBD
   ClassifyOnPermute(lay + 4);// BACD
   lay[5] = lay[9];      // pull D with a skip
   ClassifyAndPull  (lay + 2);// CABD
   ClassifyAndPull  (lay + 1);// BCAD
   ClassifyOnPermute(lay + 0);// ABCD
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
   } else {
      SignOutChunk();
   }

   // mark together all saved boards
   progress.hitsCount[IO_ROW_SELECTED][0]++;

   // any extra work
   (this->*sem.onBoardAdded)(lay);
}

void Walrus::SignOutChunk()
{
   mul.countShare = mul.countIterations + sem.scanCover;
}

void DdsTask3::Init(twContext* lay) 
{ 
   north = lay[NORTH].hand; 
   east  = lay[EAST].hand; 
   south = lay[SOUTH].hand; 
}

#ifdef SEMANTIC_SPLINTER_SHAPE
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

void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.onScanCenter = &Walrus::Scan4Hands;
   sem.onScoring = &Walrus::Score_NV6Major;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.onAfterMath = &Walrus::SolveSavedTasks;
   sem.scanCover = SYMM * PERMUTE_FACTOR;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER( NORTH, ExactShape, 4, 4, 4, 1);
   ADD_2PAR_FILTER( SOUTH, SpadesLen, 5, 6);
   ADD_2PAR_FILTER( SOUTH, PointsRange, 11, 16);
   ADD_3PAR_FILTER( NORTH, LinePointsRange, SOUTH, IO_HCP_MIN, IO_HCP_MAX);
   ADD_3PAR_FILTER( NORTH, LineControlsRange, SOUTH, 8, 8);
   ADD_3PAR_FILTER( NORTH, LineKeyCardsSpade, SOUTH, 4, 5);
   ADD_1PAR_FILTER( NORTH, ClubPointsLimit, 1);
   ADD_0PAR_FILTER( WEST,  NoOvercall );
   ADD_0PAR_FILTER( SOUTH, SpadesNatural );
   ADD_0PAR_FILTER( EAST,  No7Plus );
   ADD_0PAR_FILTER( WEST,  No2SuiterAntiSpade );

   // used previously
   // ADD_1PAR_FILTER( NORTH, PointsAtLeast, 10);
   // ADD_2PAR_FILTER( SOUTH, ControlsRange, 4, 10);
   // ADD_2PAR_FILTER( NORTH, ControlsRange, 4, 10);
   // sem.onBoardAdded = &Walrus::DisplayBoard;
   // sem.onBoardAdded = &Walrus::GrabSplinterVariant;
}
#endif // SEMANTIC_SPLINTER_SHAPE

#ifdef SEMANTIC_STANDARD_3NT
void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.onScanCenter = &Walrus::Scan4Hands;
   sem.onScoring = &Walrus::Score_3NT;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.onAfterMath = &Walrus::SolveSavedTasks;
   sem.scanCover = SYMM * PERMUTE_FACTOR;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, IO_HCP_MIN, IO_HCP_MAX);
   ADD_1PAR_FILTER(NORTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(SOUTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(NORTH, NoMajorFit, SOUTH);

   //sem.onBoardAdded = &Walrus::DisplayBoard;
}
#endif // SEMANTIC_STANDARD_3NT

#ifdef SEMANTIC_STANDARD_6NT
void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.onScanCenter = &Walrus::Scan4Hands;
   sem.onScoring = &Walrus::Score_NV6NoTrump;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.onAfterMath = &Walrus::SolveSavedTasks;
   sem.scanCover = SYMM * PERMUTE_FACTOR;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, IO_HCP_MIN, IO_HCP_MAX);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 3, 4);
   ADD_2PAR_FILTER(NORTH, ClubsLen, 3, 4);
   ADD_1PAR_FILTER(NORTH, NoMajorFit, SOUTH);
   ADD_1PAR_FILTER(NORTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(SOUTH, PointsAtLeast, 10);
   ADD_3PAR_FILTER(NORTH, LineAcesRange, SOUTH, 3, 4);

   //sem.onBoardAdded = &Walrus::DisplayBoard;
   //ADD_2PAR_FILTER(SOUTH, DiamondsLen, 5, 5);
}
#endif // SEMANTIC_STANDARD_6NT

#ifdef SEMANTIC_CONFIG_BASED
void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.onScanCenter = &Walrus::Scan4Hands;
   sem.onScoring = &Walrus::Score_3NT;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.onAfterMath = &Walrus::SolveSavedTasks;
   sem.scanCover = SYMM * PERMUTE_FACTOR;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, IO_HCP_MIN, IO_HCP_MAX);
   ADD_1PAR_FILTER(NORTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(SOUTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(NORTH, NoMajorFit, SOUTH);

   //sem.onBoardAdded = &Walrus::DisplayBoard;
}
#endif // SEMANTIC_CONFIG_BASED

