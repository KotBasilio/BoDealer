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

//#define DBG_SHOW_ALLOCS

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
   config.txt.namesBase.Build();
   mainProgress = &progress;
   PrepareLinearScores();
   memset(progress.hitsCount, 0, sizeof(progress.hitsCount));
}

void WaFilter::Bind(class Walrus* _walrus) 
{ 
   progress = _walrus->GetProgress(); 
   sem = _walrus->GetSemantics();
}

WaConfig::Contract::Contract()
   : trump(SOL_NOTRUMP)
   , goal(0)
   , by(SOUTH)
   , first(WEST)
{
   txtTrump[0] = 0;
   txtBy[0] = 0;
   txtAttacker[0] = 0;
}

WaConfig::Filters::Filters()
   : compiled()
{
   sourceCode[0] = 0;
}

WaConfig::Postmortem::Postmortem()
   : Type(WPM_NONE)
   , minHCP(0), maxHCP(0)
   , minControls(0)
{
}

int WaConfig::Postmortem::FactorFromRow(int idx)
{
    return (idx - IO_ROW_POSTMORTEM) / 2 + minHCP;
}

WaConfig::WaConfig()
   : postmSuit(0)
   , postmHand(NORTH)
{
   // texts
   txt.nameTask[0] = 0;
   txt.titleBrief[0] = 0;
   txt.primaScorerCode[0] = 0;
   txt.secundaScorerCode[0] = 0;
   txt.taskHandPBN[0] = 0;
   txt.secLongName[0] = 0;
   txt.freqTitleFormat[0] = 0;
   prim.txtTrump[0] = 0;
   prim.txtBy[0] = 0;
   prim.txtAttacker[0] = 0;
   secondary.txtTrump[0] = 0;

   // DOC: solutions parameter
   // 1 -- Find the maximum number of tricks for the side to play. Return only one of the optimum cards and its score.
   // 2 -- Find the maximum number of tricks for the side to play. Return all optimum cards and their scores.
   // 3 -- Return all cards that can be legally played, with their scores in descending order.
   #ifdef DETAILED_LEADS
      solve.ddsSol = 3;
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
   , onBoardFound        (&Walrus::AddForSolving)
   , onBoardAdded        (&MiniUI::VoidAdded)
   , onScanCenter        (&Walrus::NOP)
   , onAfterMath         (&Walrus::NOP) 
   , onPrimaryScoring    (&CumulativeScore::VoidGenScoring)
   , onSinglePrimary     (&CumulativeScore::VoidSingleScoring)
   , onSecondScoring     (&CumulativeScore::VoidGenScoring)
   , onSingleSecondary   (&CumulativeScore::VoidSingleScoring)
   , onPostmortem        (&Walrus::VoidPostmortem)
   , solveSecondTime     (&Walrus::VoidSecondSolve)
   , onCompareContracts  (&Walrus::VoidCompare)
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
   if (!MiniLink(vecFilters)) {
      isInitSuccess = false;
      printf("Semantics ERROR: Failed to link filters.\n");
      return;
   }
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

void Semantics::SetOurPrimaryScorer(CumulativeScore &cs, const char* code)
{
   if (!cs.prima.Init(cs.bidGame, code)) {
      isInitSuccess = false;
      return;
   }

   // ok
   onPrimaryScoring = &CumulativeScore::Primary;
   config.prim.goal = cs.prima.Goal();
   onSinglePrimary = &CumulativeScore::DepPrimary;
}

void Semantics::SetSecondaryScorer(CumulativeScore &cs, s64& target, const char* code)
{
   if (!cs.secunda.Init(target, code)) {
      isInitSuccess = false;
      return;
   }

   // ok
   onSecondScoring = &CumulativeScore::Secondary;
   config.secondary.goal = cs.secunda.Goal();
   onSingleSecondary = &CumulativeScore::DepSecondary;
}

void Semantics::SetOurSecondaryScorer(CumulativeScore &cs, const char* code)
{
   if (isInitSuccess) {
      SetSecondaryScorer(cs, cs.ourOther, code);
      if (!isInitSuccess) {
         printf("Semantics ERROR: the task suggest having our second scorer.\n");
      }
   }
}

void Semantics::SetTheirScorer(CumulativeScore& cs, const char* code)
{
   if (isInitSuccess) {
      SetSecondaryScorer(cs, cs.oppContract, code);
      if (!isInitSuccess) {
         printf("Semantics ERROR: the task suggest having their scorer.\n");
      }
      if (cs.secunda.HasDouble()) {
         cs.secunda.TargetOut(cs.oppCtrDoubled);
      }
   }
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
   onSinglePrimary = &CumulativeScore::BiddingLevel;
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
   if (config.IsInitFailed()) {
      return;
   }

   shuf.InitDeck();

   (this->*sem.onInit)();

   shuf.ClearFlipover();
   shuf.StoreCheckSum();
   shuf.VerifyCheckSum();
   shuf.SeedRand();

   sem.PrepareBaseDeal();
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

