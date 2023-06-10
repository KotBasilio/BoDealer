/************************************************************
 * Walrus project                                        2019
 * Initialization parts
 *
 ************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include "walrus.h"
#include "../dds-develop/src/PBN.h"
#include "../dds-develop/examples/hands.h"
#include HEADER_CURSES
#include <memory.h> // memset

//#define DBG_SHOW_ALLOCS

SplitBits sbBlank;
Semantics semShared;

Walrus::Walrus()
   // highBitscounts as many two cards in any suit. easily detected. doesn't cause an overflow
   : sem(semShared)
   , cumulScore()
   , progress()
   , ui()
   , mul()
   , shuf()
   , filter()
{
   filter.Bind(this);
   namesBase.Build();
}

void WaFilter::Bind(class Walrus* _walrus) 
{ 
   progress = _walrus->GetProgress(); 
}

bool Walrus::InitByConfig()
{
   // may read something
   const char* fname = namesBase.StartFrom; //printf("\nNo initial stats needed\n");

   // prepare basing on config
   FillSemantic();
   InitDeck();
   memset(progress.hitsCount, 0, sizeof(progress.hitsCount));
   shuf.SeedRand();

   return true;
}

void Walrus::AllocFilteredTasksBuf()
{
   // determine size
   size_t bsize = mul.maxTasksToSolve * sizeof(DdsTask);

   // alloc
   mul.arrToSolve = (DdsTask *)malloc(bsize);
   if (!mul.arrToSolve) {
      printf("%s: alloc failed\n", mul.nameHlp);
      PLATFORM_GETCH();
      exit(0);
   }

   // may report
   #ifdef DBG_SHOW_ALLOCS
      const size_t oneK = 1024;
      const size_t oneM = 1024 * oneK;
      if (bsize > oneM) {
         printf("Memory %lluM in %s\n", bsize / oneM, mul.nameHlp);
      }
      else {
         printf("Memory %lluK in %s\n", bsize / oneK, mul.nameHlp);
      }
   #endif
}

Walrus::~Walrus()
{
   if (mul.arrToSolve) {
      free(mul.arrToSolve);
      mul.arrToSolve = nullptr;
   }
}

Semantics::Semantics()
   : onInit       (&Walrus::NOP)
   , onShareStart (&Walrus::NOP)
   , fillFlipover (&Shuffler::NOP)
   , onBoardAdded (&Walrus::VoidAdded)
   , onScanCenter (&Walrus::NOP)
   , onAfterMath  (&Walrus::NOP) 
   , onDepFilter  (&WaFilter::DepRejectAll) 
   , onScoring    (&Walrus::VoidScoring)
   , onPostmortem (&Walrus::VoidPostmortem)
   , onSolvedTwice(&Walrus::VoidScoring)
   , scanCover(ACTUAL_CARDS_COUNT)
{
#ifdef SEEK_MAGIC_FLY
   onSolvedTwice = &Walrus::Score_MagicFly;
#endif // SEEK_MAGIC_FLY
   vecFilters.reserve(10);
   vecFilters.push_back( MicroFilter(&WaFilter::RejectAll) );
}

void Walrus::InitDeck(void)
{
   shuf.InitDeck();

   (this->*sem.onInit)();

   shuf.ClearFlipover();
   shuf.StoreCheckSum();
}

u64 Walrus::SumFirstHand()
{
   // sum up the first 13 cards -- kind of unrolled loop
   return (
      shuf.deck[ 0].card.jo +
      shuf.deck[ 1].card.jo +
      shuf.deck[ 2].card.jo +
      shuf.deck[ 3].card.jo +
      shuf.deck[ 4].card.jo +
      shuf.deck[ 5].card.jo +
      shuf.deck[ 6].card.jo +
      shuf.deck[ 7].card.jo +
      shuf.deck[ 8].card.jo +
      shuf.deck[ 9].card.jo +
      shuf.deck[10].card.jo +
      shuf.deck[11].card.jo +
      shuf.deck[12].card.jo
   );
}

u64 Walrus::SumSecondHand()
{
   // sum up next 13 cards
   return (
      shuf.deck[13].card.jo +
      shuf.deck[14].card.jo +
      shuf.deck[15].card.jo +
      shuf.deck[16].card.jo +
      shuf.deck[17].card.jo +
      shuf.deck[18].card.jo +
      shuf.deck[19].card.jo +
      shuf.deck[20].card.jo +
      shuf.deck[21].card.jo +
      shuf.deck[22].card.jo +
      shuf.deck[23].card.jo +
      shuf.deck[24].card.jo +
      shuf.deck[25].card.jo
   );
}

u64 Walrus::Sum3rdHand()
{
   // sum up yet next 13 cards
   return (
      shuf.deck[26].card.jo +
      shuf.deck[27].card.jo +
      shuf.deck[28].card.jo +
      shuf.deck[29].card.jo +
      shuf.deck[30].card.jo +
      shuf.deck[31].card.jo +
      shuf.deck[32].card.jo +
      shuf.deck[33].card.jo +
      shuf.deck[34].card.jo +
      shuf.deck[35].card.jo +
      shuf.deck[36].card.jo +
      shuf.deck[37].card.jo +
      shuf.deck[38].card.jo
   );
}

void Walrus::WithdrawHolding(uint hld, uint waPosByDds)
{
   // scan cards
   u64 waSuit = 1LL << waPosByDds;
   WithdrawDeuce(hld & R2, waSuit);
   WithdrawRank (hld & R3, waSuit, waPosByDds);
   WithdrawRank (hld & R4, waSuit, waPosByDds);
   WithdrawRank (hld & R5, waSuit, waPosByDds);
   WithdrawRank (hld & R6, waSuit, waPosByDds);
   WithdrawRank (hld & R7, waSuit, waPosByDds);
   WithdrawRank (hld & R8, waSuit, waPosByDds);
   WithdrawRank (hld & R9, waSuit, waPosByDds);
   WithdrawRank (hld & RT, waSuit, waPosByDds);
   WithdrawRank (hld & RJ, waSuit, waPosByDds);
   WithdrawRank (hld & RQ, waSuit, waPosByDds);
   WithdrawRank (hld & RK, waSuit, waPosByDds);
   WithdrawRank (hld & RA, waSuit, waPosByDds);
}

void Walrus::SolveSavedTasks()
{
   // a useful sum to reconstruct responder hand
   SplitBits taskSum(shuf.CheckSum());

   // how much filtered out
   u64 sum1st = 0, sum2nd = 0;
   for (int i = 0; i < 40 ; i++) {
      sum1st += progress.hitsCount[i][1] + progress.hitsCount[i][2] + progress.hitsCount[i][3];
      sum2nd += progress.hitsCount[i][4] + progress.hitsCount[i][5] + progress.hitsCount[i][6];
   }
   u64 sum = sum1st + sum2nd;

   // show filtration results
   u64 dvs = mul.countToSolve ? mul.countToSolve : 1;
   printf("Passing %u for double-dummy inspection: roughly each 1 of %llu; %llu skipped\n", mul.countToSolve, sum / dvs, sum);
   MiniReport(mul.countToSolve);

   // some hit counts are going to appear again as solved tasks
   progress.hitsCount[1][1] = 0;
   progress.hitsCount[IO_ROW_SELECTED][0] = 0;

   // do inits for Bo-Analyzer
   deal dlBase;
   PrepareBaseDeal(dlBase);
   InitMiniUI(dlBase.trump, dlBase.first);
   SetMaxThreads(0);

   // decide how to solve
   #ifdef SOLVE_ONE_BY_ONE
      SolveOneByOne(dlBase);
   #else
      SolveInChunks(dlBase);
   #endif
}
