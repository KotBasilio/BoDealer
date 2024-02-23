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
WaConfig cfgTask;

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
   cfgTask.namesBase.Build();
   mainProgress = &progress;
}

void WaFilter::Bind(class Walrus* _walrus) 
{ 
   progress = _walrus->GetProgress(); 
   sem = _walrus->GetSemantics();
}

WaConfig::WaConfig()
   : namesBase()
   , primGoal(0)
   , otherGoal(0)
   , postmSuit(0)
   , detailedReportType(WREPORT_NONE)
{
   titleOurContract [0] = 0;
   titleTheirContract[0] = 0;

   #ifdef IO_SHOW_HCP_CTRL_SPLIT
      detailedReportType = WREPORT_HCP;
   #endif
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
      } else {
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
   : onInit              (&Walrus::NOP)
   , onShareStart        (&Walrus::NOP)
   , fillFlipover        (&Shuffler::NOP)
   , onBoardAdded        (&Walrus::VoidAdded)
   , onScanCenter        (&Walrus::NOP)
   , onAfterMath         (&Walrus::NOP) 
   , onScoring           (&Walrus::VoidScoring)
   , onPostmortem        (&Walrus::VoidPostmortem)
   , solveSecondTime     (&Walrus::VoidSecondSolve)
   , onCompareContracts  (&Walrus::VoidCompare)
   , onSolvedTwice       (&Walrus::VoidScoring)
   , scanCover(ACTUAL_CARDS_COUNT)
   , dlBase(nullptr)
{
   // reject all. should analyze config later and fill 
   vecFilters.reserve(10);
   vecFilters.push_back( MicroFilter(&WaFilter::RejectAll, "RejectAll"));

   // to move to config analisys
   #ifdef SOLVE_TWICE_HANDLED_CHUNK
      solveSecondTime = &Walrus::SolveSecondTime;
   #endif

   #ifdef SEEK_SACRIFICE_DECISION
      onCompareContracts = &Walrus::NoticeSacrificePossible;
   #elif defined(SEEK_DECISION_BID_OR_DOUBLE)
      onCompareContracts = &Walrus::NoticeBidProfit;
   #elif defined(SEEK_MAGIC_FLY)
      onCompareContracts = &Walrus::NoticeMagicFly;
   #elif defined(THE_OTHER_IS_OURS)
      onCompareContracts = &Walrus::CountComboScore;
   #endif

}

void Semantics::MiniLink()
{
   // resolve any-in-list-below block
   uint retAddr;
   for (uint ip = 0; ip < vecFilters.size(); ip++) {
      // notice one
      auto& mic = vecFilters[ip];
      if (mic.func != &WaFilter::AnyInListBelow) {
         continue;
      }

      // detect return address, done on zero depth or array end
      int depth = 1;
      for (retAddr = ip+1; retAddr < vecFilters.size() && depth; retAddr++) {
         // notice nested blocks 
         const auto& scanRet = vecFilters[retAddr];
         if (scanRet.func == &WaFilter::AnyInListBelow) {
            depth++;
         }
         if (scanRet.func == &WaFilter::EndList) {
            depth--;
         }
      }

      // place it in the instruction
      mic.params[1] = retAddr - 1;
   }
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

