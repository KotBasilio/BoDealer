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
WaConfig  config;
Semantics semShared;

static const char* artWalrus[] = {
   "  __        __    _                          ___  \n",
   "  \\ \\      / /_ _| |_ __ _   _ ___          /  .\\ \n",
   "   \\ \\ /\\ / / _` | | '__| | | / __|        /  =__|\n",
   "    \\ V  V / (_| | | |  | |_| \\__ \\       /    || \n",
   "     \\_/\\_/ \\__,_|_|_|   \\__,_|___/              \n\n",
};

static void PaintWalrus()
{
   for (auto art : artWalrus) {
      printf(art);
   }
}

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
   PaintWalrus();
   filter.Bind(this);
   config.txt.namesBase.Build();
   mainProgress = &progress;
   PrepareLinearScores();
   memset(progress.hitsCount, 0, sizeof(progress.hitsCount));
   ui.FillMiniRows();
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

void WaMulti::AllocFilteredTasksBuf()
{
   // determine size
   size_t bsize = maxTasksToSolve * sizeof(WaTask);

   // alloc
   arrToSolve = (WaTask *)malloc(bsize);
   if (!arrToSolve) {
      printf("%s: alloc failed %llu bytes\n", nameHlp, bsize);
      PLATFORM_GETCH();
      exit(0);
   }

   // may report
   if (config.dbg.verboseMemory) {
      printf("Memory %s in %s\n", SizeToReadable(bsize), nameHlp);
   }
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
   , onShareStart        (&WaMulti::NOP)
   , fillFlipover        (&Shuffler::NOP)
   , onBoardFound        (&Walrus::AddForSolving)
   , onBoardAdded        (&MiniUI::VoidAdded)
   , onScanCenter        (&Walrus::NOP)
   , onPrimaryScoring    (&CumulativeScore::VoidGenScoring)
   , onSinglePrimary     (&CumulativeScore::VoidSingleScoring)
   , onSecondScoring     (&CumulativeScore::VoidGenScoring)
   , onSingleSecondary   (&CumulativeScore::VoidSingleScoring)
   , onAfterMath         (&Walrus::NOP) 
   , onMarkAfterSolve    (&Walrus::VoidFirstMarks)
   , solveSecondTime     (&Walrus::VoidSecondSolve)
   , onCompareContracts  (&Walrus::VoidCompare)
   , onSecondMarks       (&Walrus::VoidSecondMarks)
   , scanCover(ACTUAL_CARDS_COUNT)
   , dlBase(nullptr)
{
   // filters -- reject all. filled on config reading and then on MiniLink
   vecFilters.reserve(10);
   vecFilters.push_back( MicroFilter(&WaFilter::RejectAll, "RejectAll"));
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
   #ifdef SEEK_OPENING_LEAD
      solve.ddsSol = 3;
   #endif

   #ifdef SEEK_MAGIC_FLY 
      solve.shouldSolveTwice = true;
      io.showMagicFly = true;
      io.rowPercentage = IO_ROW_MAGIC_FLY;
   #elif defined(THE_OTHER_IS_OURS) || defined(SEEK_DECISION_COMPETE)
      solve.shouldSolveTwice = true;
      io.showHugeMatch = true;
      io.rowPercentage = IO_ROW_COMPARISON;
   #endif

   #ifdef SHOW_OUR_OTHER
      io.showOurOther = true;
   #endif

   #ifdef PERCENTAGES_IN_ANSWER_ROW
      io.showPercentages = true;
      #ifdef ANSWER_ROW_IDX
         io.rowPercentage = ANSWER_ROW_IDX;
      #endif
   #endif

   #ifdef SHOW_OPP_RESULTS
      io.showOppResults = true;
      #ifdef SHOW_OPPS_ON_PASS_ONLY
         io.oppsOnlyDoubled = true;
      #elif defined(SHOW_OPPS_ON_DOUBLE_ONLY)
         io.oppsOnlyDoubled = true;
      #endif
   #endif

   #ifdef SEEK_DECISION_COMPETE
      io.seekDecisionCompete = true;
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
         printf("Semantics ERROR: the task suggests having our second scorer.\n");
      }
   }
}

void Semantics::SetTheirScorer(CumulativeScore& cs, const char* code)
{
   if (isInitSuccess) {
      SetSecondaryScorer(cs, cs.oppContract, code);
      if (!isInitSuccess) {
         printf("Semantics ERROR: the task suggests having their scorer.\n");
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
   onPrimaryScoring = &CumulativeScore::BiddingLevel;
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


