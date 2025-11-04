/************************************************************
* Walrus project                                        2023
* Configuration
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include HEADER_CURSES
#include "walrus.h"
#include <assert.h>

void waFileNames::Build()
{
   // make path
   char* buf = StartFrom;
   size_t size = sizeof(StartFrom);
   #ifdef WIN_DETECT_PATH
      int rl = GetModuleFileName(NULL, buf, (DWORD)size);
      const char *key = "Walrus";
      char* in = strstr(buf, key);
      if (in) {
         *in = 0;
      }
      in = strstr(buf, "exe");
      if (in) {
         *in = 0;
      }
      in = strstr(buf, "sln");
      if (in) {
         *in = 0;
      }
   #else
      buf[0] = 0;
   #endif // WIN_DETECT_PATH

   // duplicate
   memcpy(Command, buf, size);
   memcpy(Progress, buf, size);
   memcpy(Solution, buf, size);

   // make real names
   strcat(StartFrom, START_FROM_FNAME);
   strcat(Command, COMMAND_FNAME);
   strcat(Progress, PROGRESS_FNAME);
   strcat(Solution, OUT_FNAME);
}

void Walrus::DetectGoals()
{
   // ensure we have a scorer
   if (cumulScore.prima.IsEmpty()) {
      printf("\nERROR: No primary scorer found\n");
      config.MarkFail();
      return;
   }
   config.SetupSeatsAndTrumps(cumulScore);

   // we really test & display scorers, not just a linear values
   // -- primary
   DdsTricks tr;
   char tail[128];
   CumulativeScore zeroes(cumulScore);
   owl.Show("Primary scorer (%s, %d tr):", config.prim.txtTrump, config.prim.goal);
   strcpy(tail, "  / ");
   for (tr.plainScore = 7; tr.plainScore <= 13 ; tr.plainScore++) {
      cumulScore = zeroes;
      ScoreWithPrimary(tr);
      AddScorerValues(tail);
   }
   owl.Show("%s\n", tail);

   // -- secondary
   bool shouldSkipSecunda = cumulScore.secunda.IsEmpty() || (sem.onPrimaryScoring == &CumulativeScore::BiddingLevel);
   if (!shouldSkipSecunda) {
      owl.Show("Contract-B scorer (%s, %d tr):", config.secondary.txtTrump, config.secondary.goal);
      strcpy(tail, "  / ");
      for (tr.plainScore = 7; tr.plainScore <= 13; tr.plainScore++) {
         cumulScore = zeroes;
         ScoreWithSecondary(tr);
         AddScorerValues(tail);
      }
      owl.Show("%s\n", tail);
   }

   // final cleanup
   cumulScore = zeroes;
}

void Walrus::AddScorerValues(char* tail)
{
   char chunk[20];

   if (config.postm.Is(WPM_OPENING_LEADS)) {
      if (cumulScore.ideal) {
         owl.Show(" %lld", cumulScore.ideal);
         return;
      }
   }

   if (cumulScore.oppContract) {
      owl.Show(" %lld", -cumulScore.oppContract);
      if (cumulScore.oppCtrDoubled) {
         sprintf(chunk, " %lld", -cumulScore.oppCtrDoubled);
         strcat(tail, chunk);
      }
      return;
   }

   if (cumulScore.oppCtrDoubled) {
      owl.Show(" %lld", -cumulScore.oppCtrDoubled);
      return;
   }

   if (cumulScore.bidGame) {
      owl.Show(" %lld", cumulScore.bidGame);
      if (cumulScore.bidPartscore) {
         sprintf(chunk, " %lld", cumulScore.bidPartscore);
         strcat(tail, chunk);
      }
      return;
   }

   if (cumulScore.ourOther) {
      owl.Show(" %lld", cumulScore.ourOther);
      return;
   }

   if (cumulScore.bidPartscore) {
      sprintf(chunk, " %lld", cumulScore.bidPartscore);
      strcat(tail, chunk);
      return;
   }
}

bool Walrus::InitSemantics()
{
   // FILTERS setup
   if (!config.filters.compiled.empty()) {
      sem.vecFilters = config.filters.compiled;
   }
   sem.MiniLinkFilters();

   // SCORER setup
   if (config.solve.shouldSolveTwice) {
      if (config.io.seekDecisionCompete) {
         sem.SetOurPrimaryScorer(cumulScore, config.txt.primaScorerCode);
         sem.SetTheirScorer     (cumulScore, config.txt.secundaScorerCode);
      } else {
         sem.SetOurPrimaryScorer  (cumulScore, config.txt.primaScorerCode);
         sem.SetOurSecondaryScorer(cumulScore, config.txt.secundaScorerCode);
      }
   } else {
      sem.SetBiddingLevelScorer(cumulScore);
   }

   // SOLVING setup
   if (config.solve.shouldSolveTwice) {
      sem.solveSecondTime = &Walrus::SolveSecondTime;
      sem.onCompareContracts = &Walrus::ComparePrimaSecunda;
   }

   // POSTMORTEM is about extra marks after solving
   {
      if (config.postm.Is(WPM_OPENING_LEADS)) {
         sem.SetOpeningLeadScorer(cumulScore, config.txt.primaScorerCode);
         sem.onMarkAfterSolve = &Walrus::AddMarksByOpLeads;
      }

      if (config.postm.Is(WPM_HCP_SINGLE_SCORER)) {
         assert(!config.postm.minControls);
         sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
      }

      if (config.postm.Is(WPM_CONTROLS)) {
         assert(config.postm.minControls);
         sem.onMarkAfterSolve = &Walrus::AddMarksByControls;
      }

      if (config.postm.Is(WPM_SUIT)) {
         sem.onMarkAfterSolve = &Walrus::AddMarksBySuit;
      }
   }

   // DEBUG setup
   if (config.dbg.viewBoardOnAdd) {
      sem.onBoardAdded = &MiniUI::DisplayBoard;
   }

   // many tasks fully relay on config
   if (config.solve.customSemantic) {// custom semantics have final word
      FillSemantic();
      sem.MiniLinkFilters();
   } else { 
      SemanticsToOrbitFixedHand();
   }
   if (!sem.scanCover) {
      sem.MarkFail("Don't know how much boards covers one scan");
   }

   return sem.IsInitOK();
}

void WaConfig::MakeSecondaryScrorerForBiddingLevel()
{
   char* hedge = config.txt.secundaScorerCode;
   strcpy(hedge, config.txt.primaScorerCode);
   hedge[1]--;
   FillShortScorer(hedge, txt.secundaShort);
}

void WaConfig::AnalyzeTaskType()
{
   // decide what hand is fixed and other specifics
   switch (solve.taskType) {
      case TTYPE_ONE_SIDED_BIDDING_LEVEL:
      case TTYPE_ONE_SIDED_DENOMINATION:
      case TTYPE_COMPETITIVE_GENERIC:
         deck.fixedHand = NORTH;
         break;

      case TTYPE_SEEK_OPENING_LEAD:
         deck.fixedHand = WEST;
         // DOC: solutions parameter
         // 1 -- Find the maximum number of tricks for the side to play. Return only one of the optimum cards and its score.
         // 2 -- Find the maximum number of tricks for the side to play. Return all optimum cards and their scores.
         // 3 -- Return all cards that can be legally played, with their scores in descending order.
         solve.ddsSol = 3;
         break;

      case TTYPE_NONE:
      default:
         deck.fixedHand = -1;
         solve.customSemantic = true;
         break;
   }
}

void WaConfig::InitCardsCount()
{
   // how many cards are removed from deck
   if (deck.fixedHand < 0) {
      deck.cardsRemoved = 0;
   } else { // NORTH or WEST
      deck.cardsRemoved = SYMM;
   }

   // how many cards left
   deck.cardsCount = SOURCE_CARDS_COUNT - deck.cardsRemoved;
}

bool WaConfig::OrdinaryRead(Walrus* walrus)
{
   ReadTask(walrus);
   AnalyzeTaskType();
   InitCardsCount();
   BuildNewFilters(walrus);
   ResolvePostmortemType(walrus);
   SetupOutputOptions();

   return isInitSuccess;
}

bool Walrus::InitByConfig()
{
   // may read something
   if (!config.OrdinaryRead(this)) {
      owl.Show("Failed to init configuration\n");
      return false;
   }

   // semantic preparations
   if (!InitSemantics()) {
      owl.Show("Failed to init semantics\n");
      return false;
   }

   // other preparations basing on config
   DetectGoals();
   InitDeck();
   ui.AdaptMiniRows(this);
   if (config.IsInitFailed()) {
      owl.Show("Failed on semantic-dependent init\n");
      return false;
   }

   return StartOscar();
}


