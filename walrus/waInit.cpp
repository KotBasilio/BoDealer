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
#ifdef SEMANTIC_NOV_DBL_ON_3NT
// origin: a match at home
#define INPUT_HOLDINGS  nov_3nt_doubled
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   EAST
uint nov_3nt_doubled[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
   { 0,         0,          0,         RJ | R9 | R7 | R5  } , // spades
   { 0,         0,          0,         RQ | R6 | R4  } ,      // hearts
   { 0,         0,          0,         RQ | R8  } ,           // diamonds
   { 0,         0,          0,         RQ | RJ | R7 | R5}     // clubs
};
#endif // SEMANTIC_NOV_DBL_ON_3NT

#ifdef SEMANTIC_NOV_BID_6C_OR_DBL_4S
// origin: Morozevich https://www.gambler.ru/tour/786003/protocol?id=72526447 brd 11
#define INPUT_HOLDINGS  fito_july
#define INPUT_TRUMPS    SOL_CLUBS
#define INPUT_ON_LEAD   EAST
uint fito_july[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RA | RQ ,                      0,          0,         0  } , // spades
   { RJ | R7,                       0,          0,         0  } , // hearts
   { RA | RJ | R8 ,                 0,          0,         0  } , // diamonds
   { RK | RQ | R8 | R7 | R6 | R4 ,  0,          0,         0  }   // clubs
};
#endif // SEMANTIC_NOV_BID_6C_OR_DBL_4S

#ifdef SEMANTIC_NOV_VOIDWOOD
// origin: a match at home
#define INPUT_HOLDINGS  heartsNovVoidwood
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   EAST
uint heartsNovVoidwood[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RA | R9 | R7,             0,          0,         0  } , // spades
   { RA | RQ | RJ | RT | R8 | R5 | R3, 0,  0,         0  } , // hearts
   { RA | R5 | R3 ,            0,          0,         0  } , // diamonds
   { 0,                        0,          0,         0  }   // clubs
};
#endif // SEMANTIC_NOV_VOIDWOOD

#ifdef SEMANTIC_NOV_64_AS_TWO_SUITER
// origin: http://spb.bridgesport.ru/spb/DKVkusa/p2011171/d21p.php
#define INPUT_HOLDINGS  nov64_as_two_suiter
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   EAST
uint nov64_as_two_suiter[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RT | R8 | R6 | R5 | R4 | R2, 0,      0,         0  } , // spades
   { R7 | R4,                 0,          0,         0  } , // hearts
   { R9 ,                     0,          0,         0  } , // diamonds
   { RA | RK | RQ | RJ,       0,          0,         0  }   // clubs
};
#endif // SEMANTIC_NOV_64_AS_TWO_SUITER

#ifdef SEMANTIC_SEPT_MAJORS54_18HCP
#define INPUT_HOLDINGS  majors54_18hcp
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   EAST
uint majors54_18hcp[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RK | RQ | RJ | R6 | R3,   0,          0,         0  } , // spades
   { RA | RK | R7 | R2,        0,          0,         0  } , // hearts
   { R4 ,                      0,          0,         0  } , // diamonds
   { RK | RQ | R5,             0,          0,         0  }   // clubs
};
#endif // SEMANTIC_SEPT_MAJORS54_18HCP

#ifdef SEMANTIC_AUG_MULTI_VUL
#define INPUT_HOLDINGS  multi_vul
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   EAST
uint multi_vul[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
	{ RK | R8,                  0,          0,         0  } , // spades
	{ RA | RJ | R6 | R3,        0,          0,         0  } , // hearts
	{ RK | RT | R6 | R2 ,       0,          0,         0  } , // diamonds
	{ RA | R9 | R6,             0,          0,         0  }   // clubs
};
#endif // SEMANTIC_AUG_MULTI_VUL

#ifdef SEMANTIC_AUG_SPLIT_FIT
#define INPUT_HOLDINGS  holdings_aug_split_fit
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   WEST
uint holdings_aug_split_fit[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
	{ RK | RT | R9 | R4,      0,          0,         0  } , // spades
	{ R6 | R2,                0,          0,         0  } , // hearts
	{ RQ | RT | R8 | R7 | R3 | R2,        0,          0,         0  } , // diamonds
	{ RK                ,     0,          0,         0  }   // clubs
};
#endif // SEMANTIC_AUG_SPLIT_FIT

#ifdef SEMANTIC_JULY_AUTO_FITO_PLANKTON
#define INPUT_HOLDINGS  fito_july
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   WEST
uint fito_july[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
  { RT | R7 | R6 | R2,      0,          0,         0  } , // spades
  { R8 | R4 | R3,           0,          0,         0  } , // hearts
  { RA | R9 ,               0,          0,         0  } , // diamonds
  { RK | RT | R9 | R2 ,     0,          0,         0  }   // clubs
};
#endif // SEMANTIC_JULY_AUTO_FITO_PLANKTON

#ifdef SEMANTIC_JUNE_MAX_5D_LEAD
#define INPUT_HOLDINGS  july_max_5dX_holdings
#define INPUT_TRUMPS    SOL_DIAMONDS
#define INPUT_ON_LEAD   WEST
uint july_max_5dX_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
	{ 0,         0,          0,         R3 | R2  } ,                 // spades
	{ 0,         0,          0,         RA | RQ | RT | R6 | R2  } ,  // hearts
	{ 0,         0,          0,         RA | R7 | R4  } ,            // diamonds
	{ 0,         0,          0,         RK | RQ | R7}                // clubs
};
#define CARD_LEAD_SPADE   K3
#define CARD_LEAD_HEARTS  KA
#define CARD_LEAD_DIAMD   K4
#define CARD_LEAD_CLUBS   KK
#endif // SEMANTIC_JUNE_MAX_5D_LEAD

#ifdef SEMANTIC_AUG_LEAD_VS_3H
// origin match at home
#define INPUT_HOLDINGS  aug_lead_vs_3h
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   WEST
uint aug_lead_vs_3h[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
	{ 0,         0,          0,         RQ | RT  } ,      // spades
	{ 0,         0,          0,         RQ | RT  } ,      // hearts
	{ 0,         0,          0,         RT | R7 | R6 | R5 | R4  } , // diamonds
	{ 0,         0,          0,         RA | R9 | R6 | R3 }          // clubs
};
#define CARD_LEAD_SPADE   KQ
#define CARD_LEAD_HEARTS  KQ
#define CARD_LEAD_DIAMD   K7
#define CARD_LEAD_CLUBS   K3
#endif // SEMANTIC_AUG_LEAD_VS_3H


#ifdef SEMANTIC_JUNE_LEAD_3343
// origin https://www.gambler.ru/tour/762803/protocol?id=71379460&teamid=53269
#define INPUT_HOLDINGS  july_lead4333_holdings
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint july_lead4333_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
	{ 0,         0,          0,         R9 | R8 | R7  } ,      // spades
	{ 0,         0,          0,         R9 | R8 | R7  } ,      // hearts
	{ 0,         0,          0,         RA | RJ | R6 | R5  } , // diamonds
	{ 0,         0,          0,         RJ | R4 | R2}          // clubs
};
#define CARD_LEAD_SPADE   K9
#define CARD_LEAD_HEARTS  K9
#define CARD_LEAD_DIAMD   K6
#define CARD_LEAD_CLUBS   KJ
#endif // SEMANTIC_JUNE_LEAD_3343

#ifdef SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
// origin match at home
#define INPUT_HOLDINGS  julyVZ_Axx_holdings
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint julyVZ_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
   { 0,         0,          0,         RA | RQ | R8  } ,      // spades
   { 0,         0,          0,         RJ | R7 | R4 | R2  } , // hearts
   { 0,         0,          0,         RJ | R9 | R6 | R4  } , // diamonds
   { 0,         0,          0,         R9 | R4  }             // clubs
};
uint julyVZ_Axx_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
   { 0,         0,          0,         RA | R8 | R2  } ,      // spades
   { 0,         0,          0,         RJ | R7 | R4 | R2  } , // hearts
   { 0,         0,          0,         RJ | R9 | R6 | R4  } , // diamonds
   { 0,         0,          0,         R9 | R4  }             // clubs
};
#define CARD_LEAD_SPADE   KA
#define CARD_LEAD_HEARTS  K2
#define CARD_LEAD_DIAMD   K4
#define CARD_LEAD_CLUBS   K9
#endif // SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT

#ifdef SEMANTIC_IMPS_LEAD_LEVKOVICH
// origin http://spb.bridgesport.ru/spb/SpbImp20/p2011281/d19p.php
#define INPUT_HOLDINGS  novLevk_holdings
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint novLevk_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
   { 0,         0,          0,         R8 | R7 | R4  } ,      // spades
   { 0,         0,          0,         RQ | RT | R9 | R7  } , // hearts
   { 0,         0,          0,         RQ | RT | R9 | R7  } , // diamonds
   { 0,         0,          0,         RT | R5  }             // clubs
};
#define CARD_LEAD_SPADE   K8
#define CARD_LEAD_HEARTS  KT
#define CARD_LEAD_DIAMD   KT
#define CARD_LEAD_CLUBS   KT
#endif // SEMANTIC_IMPS_LEAD_LEVKOVICH

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

#ifdef SINGLE_HAND_TASK
   #define INPUT_HOLDINGS  dummy_holdings
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   EAST
   uint dummy_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RT | R4,                  0,          0,         0  } , // spades
      { RK | R9 | R7 | R5 | R2,   0,          0,         0  } , // hearts
      { RK | R8 | R7 | R6 | R5,   0,          0,         0  } , // diamonds
      { R6 ,                      0,          0,         0  }   // clubs
   };
#endif // SINGLE_HAND_TASK

#ifdef SEMANTIC_SPADE_4_WHEN_1H
   #define INPUT_HOLDINGS  spade_4_holdings
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   EAST
   uint spade_4_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | R7 | R5 | R4,       0,          0,         0  } , // spades
      { RK | R9 ,            0,          0,         0  } , // hearts
      { RK | R6 | R5,            0,          0,         0  } , // diamonds
      { RQ | RJ | R6 | R2,           0,          0,         0  }   // clubs
   };
#endif // SEMANTIC_SPADE_4_WHEN_1H

uint(*input_holdings)[DDS_HANDS][DDS_SUITS] = &INPUT_HOLDINGS;

SplitBits sbBlank;

void DdsTricks::Init(futureTricks &fut)
{
   // plainScore is good for any goal
   plainScore = 13 - fut.score[0];

   // the rest is for opening lead
#ifdef SEEK_OPENING_LEAD
   for (int i = 0; i < fut.cards; i++) {
      if (fut.suit[i] == SOL_SPADES && fut.rank[i] == CARD_LEAD_SPADE) {
         lead.S = 13 - fut.score[i];
         continue;
      }
      if (fut.suit[i] == SOL_HEARTS && fut.rank[i] == CARD_LEAD_HEARTS) {
         lead.H = 13 - fut.score[i];
         continue;
      }
      if (fut.suit[i] == SOL_DIAMONDS && fut.rank[i] == CARD_LEAD_DIAMD) {
         lead.D = 13 - fut.score[i];
         continue;
      }
      if (fut.suit[i] == SOL_CLUBS && fut.rank[i] == CARD_LEAD_CLUBS) {
         lead.Ñ = 13 - fut.score[i];
         continue;
      }
   }
#endif // SEEK_OPENING_LEAD
}

Walrus::Walrus()
   : isRunning(true)
   , countIterations(0)
   , countOppContractMarks(0)
   , countShare(MAX_ITERATION)
   , countSolo(0)
   , maxTasksToSolve(MAX_TASKS_TO_SOLVE)
   // highBitscounts as many two cards in any suit. easily detected. doesn't cause an overflow
   , highBits(HIBITS)  
   , checkSum(0)
   , oldRand(0)
   , nameHlp("main")
   , arrToSolve(nullptr)
   , countToSolve(0)
   , cumulScore()
{
    BuildFileNames();
    FillSemantic();
    InitDeck();
    memset(hitsCount, 0, sizeof(hitsCount));
    SeedRand();
}


Walrus::MiniUI::MiniUI()
   : exitRequested(false)
   , firstAutoShow(true)
   , irGoal(0)
   , irBase(0)
{
}

void Walrus::AllocFilteredTasksBuf()
{
   // determine size
   size_t bsize = maxTasksToSolve * sizeof(DdsTask);

   // alloc
   arrToSolve = (DdsPack *)malloc(bsize);
   if (arrToSolve) {
      const size_t oneK = 1024;
      const size_t oneM = 1024 * oneK;
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
   : onInit       (&Walrus::NOP)
   , onShareStart (&Walrus::NOP)
   , fillFlipover (&Walrus::NOP)
   , onScanCenter (&Walrus::NOP)
   , onAfterMath  (&Walrus::NOP) 
   , onFilter     (&Walrus::FilterRejectAll) 
   , onScoring    (&Walrus::VoidScoring)
   , onOppContract(&Walrus::VoidScoring)
   , scanCover(ACTUAL_CARDS_COUNT)
{
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

void Walrus::PrepareBaseDeal(deal &dlBase)
{
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

   // show filtration results
   int dvs = countToSolve ? countToSolve : 1;
   printf("Passing %u for double-dummy inspection: roughly each 1 of %llu; %llu skipped\n", countToSolve, sum / dvs, sum);
   hitsCount[1][1] = 0;
   MiniReport(countToSolve);

   // do inits for Bo-Analyzer
   deal dlBase;
   PrepareBaseDeal(dlBase);
   SetMaxThreads(0);
   InitMiniUI();

   // decide how to solve
   #ifdef _DEBUG
      //SolveOneByOne(dlBase);
      SolveInChunks(dlBase);
   #else
      SolveInChunks(dlBase);
   #endif // _DEBUG
}

