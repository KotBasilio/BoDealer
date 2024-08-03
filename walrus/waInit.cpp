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
#include "waSemMicro.h"

#define DBG_SHOW_ALLOCS

SplitBits sbBlank;
Semantics semShared;
WaConfig config;

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
   config.namesBase.Build();
   mainProgress = &progress;
   PrepareLinearScores();
}

void WaFilter::Bind(class Walrus* _walrus) 
{ 
   progress = _walrus->GetProgress(); 
   sem = _walrus->GetSemantics();
}

WaConfig::WaConfig()
   : namesBase()
   , primGoal(0)
   , secGoal(0)
   , postmSuit(0)
   , postmHand(NORTH)
   , minControls(0)
   , detailedReportType(WREPORT_NONE)
   , countFilters(0)
{
   titleBrief[0] = 0;
   primaScorerCode[0] = 0;
   secundaScorerCode[0] = 0;

   #ifdef IO_SHOW_HCP_CTRL_SPLIT
      detailedReportType = WREPORT_HCP;
   #endif

   #if IO_HCP_MIN == IO_HCP_MAX
         minControls = (IO_HCP_MIN * 4) / 10 - 6;
   #endif

   #ifdef SEEK_OPENING_LEAD
      detailedReportType = WREPORT_OPENING_LEADS;
   #endif
}

void Walrus::AllocFilteredTasksBuf()
{
   // determine size
   size_t bsize = mul.maxTasksToSolve * sizeof(WaTask);

   // alloc
   mul.arrToSolve = (WaTask *)malloc(bsize);
   if (!mul.arrToSolve) {
      printf("%s: alloc failed %llu bytes\n", mul.nameHlp, bsize);
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
   , onPrimaryScoring    (&CumulativeScore::VoidScoring)
   , onSecondScoring     (&CumulativeScore::VoidScoring)
   , onPostmortem        (&Walrus::VoidPostmortem)
   , solveSecondTime     (&Walrus::VoidSecondSolve)
   , onCompareContracts  (&Walrus::VoidCompare)
   , scanCover(ACTUAL_CARDS_COUNT)
   , dlBase(nullptr)
   , onDepPrimaryScoring (&CumulativeScore::VoidDepScoring)
   , onDepSecondScoring  (&CumulativeScore::VoidDepScoring)
{
   // reject all. should analyze config later and fill 
   vecFilters.reserve(10);
   vecFilters.push_back( MicroFilter(&WaFilter::RejectAll, "RejectAll"));

   // to move to config analisys
   #ifdef SOLVE_TWICE_HANDLED_CHUNK
      solveSecondTime = &Walrus::SolveSecondTime;
   #endif

   #ifdef SEEK_DECISION_COMPETE
      onCompareContracts = &Walrus::NoticeBidProfit;
   #elif defined(SEEK_MAGIC_FLY)
      onCompareContracts = &Walrus::NoticeMagicFly;
   #elif defined(THE_OTHER_IS_OURS)
      onCompareContracts = &Walrus::CompareOurContracts;
   #endif

}

void Semantics::MiniLinkFilters()
{
   // relay
   MiniLink(vecFilters);

   // update config
   config.countFilters = vecFilters.size();
}

bool Semantics::IsListStart(const MicroFilter& mic)
{
   return mic.func == &WaFilter::AnyInListBelow ||
          mic.func == &WaFilter::ExcludeCombination;
}

bool Semantics::IsOpeningBracket(int idx)
{
   if (idx < 0 || vecFilters.size() <= idx) {
      return false;
   }

   return IsListStart(vecFilters[idx]);
}

bool Semantics::IsClosingBracket(int idx)
{
   if (idx < 0 || vecFilters.size() <= idx) {
      return false;
   }

   return vecFilters[idx].func == &WaFilter::EndList;
}

static int GetGoalFrom(const char* code)
{
   uint level = code[1] - '0';
   return 6 + level;
}

void Semantics::SetOurPrimaryScorer(CumulativeScore &cs, const char* code)
{
   if (!cs.prima.Init(cs.bidGame, code)) {
      isInitSuccess = false;
      return;
   }

   // ok
   onPrimaryScoring = &CumulativeScore::Primary;
   config.primGoal = GetGoalFrom(code);
   onDepPrimaryScoring = &CumulativeScore::DepPrimary;
}

void Semantics::SetSecondaryScorer(CumulativeScore &cs, s64& target, const char* code)
{
   if (!cs.secunda.Init(target, code)) {
      isInitSuccess = false;
      return;
   }

   // ok
   onSecondScoring = &CumulativeScore::Secondary;
   config.secGoal = GetGoalFrom(code);
   onDepSecondScoring = &CumulativeScore::DepSecondary;
}

void Semantics::SetOurSecondaryScorer(CumulativeScore &cs, const char* code)
{
   SetSecondaryScorer(cs, cs.ourOther, code);
}

void Semantics::SetTheirScorer(CumulativeScore& cs, const char* code)
{
   SetSecondaryScorer(cs, cs.oppContract, code);
}

void Semantics::SetBiddingGameScorer(CumulativeScore& cs, const char* code)
{
   // setup both scorers
   SetOurPrimaryScorer(cs, code);

   static char codePartscore[8];
   strcpy(codePartscore, code);
   codePartscore[1]--;
   SetOurSecondaryScorer(cs, codePartscore);

   cs.secunda.TargetOut(cs.bidPartscore);
   if (!isInitSuccess) {
      return;
   }

   // allow to compare
   onDepPrimaryScoring = &CumulativeScore::BiddingLevel;
}

void Semantics::SetOpeningLeadScorer(CumulativeScore& cs, const char* code)
{
   SetOurPrimaryScorer(cs, code);
   cs.prima.TargetOut(cs.ideal);
   if (!isInitSuccess) {
      return;
   }

   // allow to count for all leads
   onPrimaryScoring = &CumulativeScore::OpeningLead;
}

void Walrus::InitDeck(void)
{
   shuf.InitDeck();

   (this->*sem.onInit)();

   shuf.ClearFlipover();
   shuf.StoreCheckSum();
   shuf.VerifyCheckSum();
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

