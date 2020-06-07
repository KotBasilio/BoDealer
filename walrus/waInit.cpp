/************************************************************
 * Walrus project                                        2019
 * Initialization parts
 *
 ************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include "walrus.h"
#include "..\dds-develop\include\dll.h"
#include "..\dds-develop\examples\hands.h"
#include <conio.h>
#include <memory.h> // memset

 // --------------------------------------------------------------------------------
 // input
#ifdef SEMANTIC_TRICOLOR_STRONG
   #define INPUT_HOLDINGS  tri_sunday
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   WEST
   uint tristrong_holdings[DDS_HANDS][DDS_SUITS] =   // 21 hcp
   { // North                    East        South       West
      { RA | RK | R8 | R5,      0,          0,         0  } , // spades
      { RK | RT | R7 | R5,      0,          0,         0  } , // hearts
      { RA ,                    0,          0,         0  } , // diamonds
      { RA | RK | R8 |  R5 ,    0,          0,         0  }   // clubs
   };
   uint tri19_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RK | R8 | R5,      0,          0,         0  } , // spades
      { RK | RT | R7 | R5,      0,          0,         0  } , // hearts
      { RA ,                    0,          0,         0  } , // diamonds
      { RA | RJ | R8 | R5 ,     0,          0,         0  }   // clubs
   };
   uint tri18_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RK | R8 | R5,      0,          0,         0  } , // spades
      { RK | RT | R7 | R5,      0,          0,         0  } , // hearts
      { RA ,                    0,          0,         0  } , // diamonds
      { RA | R9 | R8 | R5 ,     0,          0,         0  }   // clubs
   };
   uint tri16_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RJ | R8 | R5,      0,          0,         0  } , // spades
      { RK | RT | R7 | R5,      0,          0,         0  } , // hearts
      { RA ,                    0,          0,         0  } , // diamonds
      { RA | R9 | R8 | R5 ,     0,          0,         0  }   // clubs
   };
   uint tri16_c6_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RK | R8 | R5,      0,          0,         0  } , // spades
      { RK | RJ | R7 | R5,      0,          0,         0  } , // hearts
      { R8 ,                    0,          0,         0  } , // diamonds
      { RA | RJ | R8 | R5 ,     0,          0,         0  }   // clubs
   };
   //AKJ9
   //A764
   //J
   //A1032
   uint tri_sunday[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RK | RJ | R9,      0,          0,         0  } , // spades
      { RA | R7 | R6 | R4,      0,          0,         0  } , // hearts
      { RJ ,                    0,          0,         0  } , // diamonds
      { RA | RT | R3 | R2 ,     0,          0,         0  }   // clubs
   };
#endif // SEMANTIC_RED55_KINGS_PART_15_16

#ifdef SEMANTIC_RED55_KINGS_PART_15_16
   #define INPUT_HOLDINGS  red55_holdings
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   EAST
   uint red55_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RT | R4,                  0,          0,         0  } , // spades
      { RK | R9 | R7 | R5 | R2,   0,          0,         0  } , // hearts
      { RK | R8 | R7 | R6 | R5,   0,          0,         0  } , // diamonds
      { R6 ,                      0,          0,         0  }   // clubs
   };
#endif // SEMANTIC_RED55_KINGS_PART_15_16

uint(*input_holdings)[DDS_HANDS][DDS_SUITS] = &INPUT_HOLDINGS;

SplitBits sbBlank;

Walrus::Walrus()
   : isRunning(true)
   , exitRequested(false)
   , countIterations(0)
   , countShare(MAX_ITERATION)
   , countSolo(0)
   // highBitscounts as many two cards in any suit. easily detected. doesn't cause an overflow
   , highBits(HIBITS)  
   , checkSum(0)
   , oldRand(0)
   , nameHlp("main")
   , arrToSolve(nullptr)
   , countToSolve(0)
   , cumulScore()
   , irGoal(0)
{
    BuildFileNames();
    FillSemantic();
    InitDeck();
    memset(hitsCount, 0, sizeof(hitsCount));
    SeedRand();
}

void Walrus::AllocFilteredTasksBuf()
{
   size_t bsize = MAX_TASKS_TO_SOLVE * sizeof(DdsTask);
   arrToSolve = (DdsPack *)malloc(bsize);
   if (arrToSolve) {
      size_t oneK = 1024;
      size_t oneM = 1024 * oneK;
      if (bsize > oneM) {
         printf("Memory %lluM in %s\n", bsize / oneM, nameHlp);
      } else {
         printf("Memory %lluK in %s\n", bsize / oneK, nameHlp);
      }
      return;
   }

   // fail
   printf("%s: alloc failed\n", nameHlp);
   _getch();
   exit(0);
}

Walrus::~Walrus()
{
   if (arrToSolve) {
      free(arrToSolve);
      arrToSolve = nullptr;
   }
}

Walrus::Semantics::Semantics()
   : onInit      (&Walrus::NOP)
   , onShareStart(&Walrus::NOP)
   , fillFlipover(&Walrus::NOP)
   , onScanCenter(&Walrus::NOP)
   , onAfterMath (&Walrus::NOP) 
   , scanCover(ACTUAL_CARDS_COUNT)
{
}

SaDeck::SaDeck()
{
   for (int i = 0; i < COUNT_AI_CARDS ; i++)
   {
      arr[i] = sbBlank;
   }
}

void Walrus::InitDeck(void)
{
   int i = 0;
   i = InitSuit(CLUBS, i);
   i = InitSuit(DIAMD, i);
   i = InitSuit(HEART, i);
   i = InitSuit(SPADS, i);

   (this->*sem.onInit)();

   ClearFlipover();

   checkSum = CalcCheckSum();
}

u64 Walrus::CalcCheckSum()
{
    u64 jo = 0;
    for each (SplitBits sb in deck) {
        jo += sb.card.jo;
    }
    return jo;
}

void Walrus::VerifyCheckSum()
{
#ifdef _DEBUG
    if (CalcCheckSum() != checkSum) {
        printf("\nFatal -- checksum failure\n");
        for(;;) {}
    }
#endif
}

void Walrus::ClearFlipover()
{
    for (int i = FLIP_OVER_START_IDX; i < DECK_ARR_SIZE; i++) {
        deck[i] = sbBlank;
    }
}

u64 Walrus::SumFirstHand()
{
   // sum up the first 13 cards -- kind of unrolled loop
   return (
      deck[ 0].card.jo +
      deck[ 1].card.jo +
      deck[ 2].card.jo +
      deck[ 3].card.jo +
      deck[ 4].card.jo +
      deck[ 5].card.jo +
      deck[ 6].card.jo +
      deck[ 7].card.jo +
      deck[ 8].card.jo +
      deck[ 9].card.jo +
      deck[10].card.jo +
      deck[11].card.jo +
      deck[12].card.jo
   );
}

u64 Walrus::SumSecondHand()
{
   // sum up the first 13 cards
   return (
      deck[13].card.jo +
      deck[14].card.jo +
      deck[15].card.jo +
      deck[16].card.jo +
      deck[17].card.jo +
      deck[18].card.jo +
      deck[19].card.jo +
      deck[20].card.jo +
      deck[21].card.jo +
      deck[22].card.jo +
      deck[23].card.jo +
      deck[24].card.jo +
      deck[25].card.jo
   );
}

// -----------------------------------------------------------------------
// input to walrus
void Walrus::WithdrawCard(u64 jo)
{
   int high = SOURCE_CARDS_COUNT;
   while (deck[high].IsBlank()) {
      high--;
   }

   for (uint i = 0; i < SOURCE_CARDS_COUNT; i++) {
      if (deck[i].card.jo == jo) {
         deck[i] = deck[high];
         deck[high--] = sbBlank;
         return;
      }
   }

   DEBUG_UNEXPECTED;
   printf("card to withdraw is not found.\n");
}

void Walrus::WithdrawDeuce(uint rankBit, u64 waSuit)
{
   // deuce has no bit
   if (rankBit) {
      WithdrawCard(waSuit);
   }
}

void Walrus::WithdrawRank(uint rankBit, u64 waSuit, uint waSuitByDds)
{
   // other bits positions are 1 pos up from DDS
   if (rankBit) {
      u64 waBit = ((u64)rankBit) << (waSuitByDds + 1);
      WithdrawCard(waSuit + waBit);
   }
}

u64  wa_SuitByDds[DDS_SUITS] = { SPADS, HEART, DIAMD, CLUBS };
uint wa_PosByDds [DDS_SUITS] = {    48,    32,    16,     0 };
void Walrus::WithdrawByInput(void)
{
   for (int h = 0; h < DDS_HANDS; h++) {
      for (int s = 0; s < DDS_SUITS; s++) {
         auto hld = (*input_holdings)[s][h];
         WithdrawHolding(hld, wa_PosByDds[s]);
      }
   }
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
   SplitBits sbSum(checkSum);
   DdsTask::DTUnion taskSum;
   taskSum.Init(sbSum);

   // how much filtered out
   u64 sum1st = 0, sum2nd = 0;
   for (int i = 0; i < 20 ; i++) {
      sum1st += hitsCount[i][1];
      sum2nd += hitsCount[i][2];
   }
   u64 sum = sum1st + sum2nd;

   int dvs = countToSolve ? countToSolve : 1;
   printf("Passing %u for double-dummy inspection: roughly each 1 of %llu; %llu skipped\n", countToSolve, sum / dvs, sum);
   hitsCount[1][1] = 0;

   SetMaxThreads(0);

   // prepare base deal
   deal dlBase;
   dlBase.trump = INPUT_TRUMPS;
   dlBase.first = INPUT_ON_LEAD;

   dlBase.currentTrickSuit[0] = 0;
   dlBase.currentTrickSuit[1] = 0;
   dlBase.currentTrickSuit[2] = 0;

   dlBase.currentTrickRank[0] = 0;
   dlBase.currentTrickRank[1] = 0;
   dlBase.currentTrickRank[2] = 0;

   for (int h = 0; h < DDS_HANDS; h++)
      for (int s = 0; s < DDS_SUITS; s++)
         dlBase.remainCards[h][s] = (*input_holdings)[s][h];

   // decide how to solve
   #ifdef _DEBUG
      //SolveOneByOne(dlBase);
      SolveInChunks(dlBase);
   #else
      SolveInChunks(dlBase);
   #endif // _DEBUG
}

