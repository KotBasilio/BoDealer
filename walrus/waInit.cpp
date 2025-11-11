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
#include "waSemantic.h"

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
   txtCode[0] = 0;
}

WaConfig::Filters::Filters()
   : compiled()
{
   sourceCode[0] = 0;
}

bool WaConfig::Filters::FindHCPRange(int hand, int& from, int& to)
{
   for (uint ip = 0; ip < compiled.size(); ip++) {
      auto& mic = compiled[ip];
      if (mic.params[0] == hand &&
          mic.func == &WaFilter::PointsRange) {
         from = mic.params[1];
         to = mic.params[2];
         return true;
      }
   }

   return false;
}

WaConfig::Postmortem::Postmortem()
   : Type(WPM_NONE)
   , idxHand(NORTH)
   , idxSuit(SOL_HEARTS)
   , minHCP(0), maxHCP(0)
   , minControls(0)
{
   hcpFixedHand.Zero();
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
   , onSecondScoring     (&CumulativeScore::VoidGenScoring)
   , onAfterMath         (&Walrus::NOP) 
   , onFirstMarks        (&Walrus::VoidFirstMarks)
   , solveSecondTime     (&Walrus::VoidSecondSolve)
   , onCompareContracts  (&Walrus::VoidCompare)
   , onTrumpFill         (&WaConfig::AllLenses::SimpleSecondary)
   , flipSecondScorer    (&Walrus::VoidFlipper)
{
   // filters -- reject all. filled on config reading and then on MiniLink
   vecFilters.reserve(10);
   vecFilters.push_back( MicroFilter(&WaFilter::RejectAll, "RejectAll"));
}

WaConfig::WaConfig()
{
   // texts
   txt.nameTask[0] = 0;
   txt.titleBrief[0] = 0;
   txt.primaScorerCode[0] = 0;
   txt.secundaScorerCode[0] = 0;
   txt.taskHandPBN[0] = 0;
   txt.secLongName[0] = 0;
   txt.freqTitleFormat[0] = 0;
}

void WaConfig::SetupOutputOptions()
{
   // i/o : display options
   if (solve.shouldSolveTwice) {
      io.showHugeMatch = true;
      io.rowPercentage = IO_ROW_COMPARISON;

      if (solve.TheOtherIsOurs()) {
         io.showOurOther = true;
      } else {
         io.showOppResults = true;
      }
   }

   #ifdef PERCENTAGES_IN_ANSWER_ROW
      io.showPercentages = true;
      #ifdef SINGLE_HAND_TASK
         io.rowPercentage = IO_ROW_OUR_MADE;
      #endif
   #endif
}

void Semantics::MiniLinkFilters()
{
   // relay
   if (!MiniLink(vecFilters)) {
      MarkFail("Failed to link filters");
      return;
   }

   // sanity
   if (vecFilters.empty()) {
      MarkFail("No filters at all");
      return;
   }
   if (vecFilters.size() == 1) {
      if (vecFilters[0].func == &WaFilter::RejectAll) {
         MarkFail("Most likely FILTERS keyword is missing");
         return;
      }
   }
}

void Semantics::MarkFail(const char* reason) 
{ 
   isInitSuccess = false;
   auto safeReason = reason ? reason : "..";
   printf("Semantics ERROR: %s.\n", safeReason);
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

