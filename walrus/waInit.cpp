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

SplitBits sbBlank;

Walrus::Walrus()
   // highBitscounts as many two cards in any suit. easily detected. doesn't cause an overflow
   : sem()
   , cumulScore()
   , progress()
   , ui()
   , mul()
   , shuf()
   , filter()
{
   BuildFileNames();
   FillSemantic();
   InitDeck();
   memset(progress.hitsCount, 0, sizeof(progress.hitsCount));
   SeedRand();
}


Walrus::MiniUI::MiniUI()
   : exitRequested(false)
   , firstAutoShow(true)
   , irGoal(0)
   , irBase(0)
   , irFly(0)
{
}

void Walrus::AllocFilteredTasksBuf()
{
   // determine size
   size_t bsize = mul.maxTasksToSolve * sizeof(DdsTask);

   // alloc
   mul.arrToSolve = (DdsPack *)malloc(bsize);
   if (mul.arrToSolve) {
      // const size_t oneK = 1024;
      // const size_t oneM = 1024 * oneK;
      // if (bsize > oneM) {
      //    printf("Memory %lluM in %s\n", bsize / oneM, nameHlp);
      // } else {
      //    printf("Memory %lluK in %s\n", bsize / oneK, nameHlp);
      // }
      return;
   }

   // fail
   printf("%s: alloc failed\n", mul.nameHlp);
   PLATFORM_GETCH();
   exit(0);
}

Walrus::~Walrus()
{
   if (mul.arrToSolve) {
      free(mul.arrToSolve);
      mul.arrToSolve = nullptr;
   }
}

Walrus::Semantics::Semantics()
   : onInit       (&Walrus::NOP)
   , onShareStart (&Walrus::NOP)
   , fillFlipover (&Walrus::NOP)
   , onScanCenter (&Walrus::NOP)
   , onAfterMath  (&Walrus::NOP) 
   , onFilter     (&WaFilter::RejectAll) 
   , onScoring    (&Walrus::VoidScoring)
   , onSolvedTwice(&Walrus::VoidScoring)
   , scanCover(ACTUAL_CARDS_COUNT)
{
#ifdef SEEK_MAGIC_FLY
   onSolvedTwice = &Walrus::Score_MagicFly;
#endif // SEEK_MAGIC_FLY
}

void Walrus::InitDeck(void)
{
   int i = 0;
   i = InitSuit(CLUBS, i);
   i = InitSuit(DIAMD, i);
   i = InitSuit(HEART, i);
   i = InitSuit(SPADS, i);
   shuf.cardsInDeck = i;

   (this->*sem.onInit)();

   ClearFlipover();

   shuf.checkSum = CalcCheckSum();
}

u64 Walrus::CalcCheckSum()
{
    u64 jo = 0;
    for (auto sb : shuf.deck) {
        jo += sb.card.jo;
    }
    return jo;
}

void Walrus::VerifyCheckSum()
{
#ifdef _DEBUG
    if (CalcCheckSum() != shuf.checkSum) {
        printf("\nFatal -- checksum failure\n");
        for(;;) {}
    }
#endif
}

void Walrus::ClearFlipover()
{
    for (int i = FLIP_OVER_START_IDX; i < DECK_ARR_SIZE; i++) {
        shuf.deck[i] = sbBlank;
    }
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
   // sum up the first 13 cards
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
   SplitBits sbSum(shuf.checkSum);
   DdsTask::DTUnion taskSum;
   taskSum.Init(sbSum);

   // how much filtered out
   u64 sum1st = 0, sum2nd = 0;
   for (int i = 0; i < 20 ; i++) {
      sum1st += progress.hitsCount[i][1];
      sum2nd += progress.hitsCount[i][2];
   }
   u64 sum = sum1st + sum2nd;

   // show filtration results
   int dvs = mul.countToSolve ? mul.countToSolve : 1;
   printf("Passing %u for double-dummy inspection: roughly each 1 of %llu; %llu skipped\n", mul.countToSolve, sum / dvs, sum);
   progress.hitsCount[1][1] = 0;
   MiniReport(mul.countToSolve);

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
