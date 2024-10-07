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
      } else if (cumulScore.bidSlam) {
         sprintf(chunk, " %lld", cumulScore.bidSlam);
         strcat(tail, chunk);
      }
      return;
   }

   if (cumulScore.ourOther) {
      owl.Show("    ");
      sprintf(chunk, " %lld", cumulScore.ourOther);
      strcat(tail, chunk);
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
   // many tasks fully relay on config
   #ifdef SEM_ORBITING_FIXED_HAND
      SemanticsToOrbitFixedHand();
   #else // other tasks, older approach
      FillSemantic();
   #endif

   // common part
   if (!config.filters.compiled.empty()) {
      sem.vecFilters = config.filters.compiled;
   }
   sem.MiniLinkFilters();

   #ifdef SEEK_DECISION_COMPETE
      sem.SetOurPrimaryScorer(cumulScore, config.txt.primaScorerCode);
      sem.SetTheirScorer(cumulScore, config.txt.secundaScorerCode);
   #endif

   #ifdef SEEK_DENOMINATION
      sem.SetOurPrimaryScorer(cumulScore, config.txt.primaScorerCode);
      sem.SetOurSecondaryScorer(cumulScore, config.txt.secundaScorerCode);
   #endif

   #ifdef SEEK_BIDDING_LEVEL
      sem.SetBiddingGameScorer(cumulScore, config.txt.primaScorerCode);
   #endif

   // POSTMORTEM setup
   {
      if (config.postm.Is(WPM_OPENING_LEADS)) {
         sem.SetOpeningLeadScorer(cumulScore, config.txt.primaScorerCode);
         sem.onMarkAfterSolve = &Walrus::AddMarksByOpLeads;
      }

      if (config.postm.Is(WPM_HCP_SINGLE_SCORER)) {
         sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
         if (config.postm.minHCP == config.postm.maxHCP) {
            // TODO: make a separate function for controls. Now it's same, &Walrus::AddMarksByHCP
         }
      }

      if (config.postm.Is(WPM_SUIT)) {
         sem.onMarkAfterSolve = &Walrus::AddMarksBySuit;
      }
   }

   // DEBUG setup
   if (config.dbg.viewBoardOnAdd) {
      sem.onBoardAdded = &MiniUI::DisplayBoard;
   }

   return sem.IsInitOK();
}

bool Walrus::InitByConfig()
{
   // may read something
   config.ReadTask(this);
   config.BuildNewFilters(this);
   if (config.IsInitFailed()) {
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

