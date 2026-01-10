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
   if (config.cli.nameFileConfig[0]) {
      strcpy(StartFrom, config.cli.nameFileConfig);
   } else {
      strcat(StartFrom, START_FROM_FNAME);
   }
   strcat(Command, COMMAND_FNAME);
   strcat(Progress, PROGRESS_FNAME);
   strcat(Solution, OUT_FNAME);
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
      if (config.solve.seekDecisionCompete) {
         sem.SetOurPrimaryScorer(cumulScore, config.lens.prim.txtCode);
         sem.SetTheirScorer     (cumulScore, config.lens.secondary.txtCode);
      } else {
         sem.SetOurPrimaryScorer  (cumulScore, config.lens.prim.txtCode);
         sem.SetOurSecondaryScorer(cumulScore, config.lens.secondary.txtCode);
      }
   } else {
      sem.SetBiddingLevelScorer(cumulScore);
   }

   // SOLVING setup
   if (config.solve.shouldSolveTwice) {
      sem.solveSecondTime = &Walrus::SolveSecondTime;
      sem.onCompareContracts = &Walrus::ComparePrimaSecunda;
      if (config.lens.IsManyLenses()) {
         sem.onTrumpFill = &WaConfig::AllLenses::TrumpFillMultiLens;
         sem.flipSecondScorer = &Walrus::FlipSecByMultiLens;
      }
   }

   // POSTMORTEM is about extra marks after solving
   {
      if (config.postm.Is(WPM_OPENING_LEADS)) {
         sem.SetOpeningLeadScorer(cumulScore, config.lens.prim.txtCode);
         sem.onFirstMarks = &Walrus::AddMarksByOpLeads;
      }

      if (config.postm.Is(WPM_HCP_SINGLE_SCORER)) {
         assert(!config.postm.minControls);
         sem.onFirstMarks = &Walrus::AddMarksByHCP;
      }

      if (config.postm.Is(WPM_CONTROLS)) {
         assert(config.postm.minControls);
         sem.onFirstMarks = &Walrus::AddMarksByControls;
      }

      if (config.postm.Is(WPM_SUIT)) {
         sem.onFirstMarks = &Walrus::AddMarksBySuit;
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

void WaConfig::AnalyzeTaskType()
{
   // decide what hand is fixed and other specifics
   switch (solve.taskType) {
      case TTYPE_FIXED_ONE_HAND:
         solve.taskType = DetectOneHandVariant();
         owl.Show("Recognized a fixed hand task type: %s\n", TaskTypeText(solve.taskType));
         AnalyzeTaskType();
         break;

      // one hand variants
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
         if (isInitSuccess) {
            printf("Error: '%s' line is missing.\n", key.TType);
            MarkFail();
         }
         break;

      case TTYPE_FOUR_HANDS_TASK:
      case TTYPE_FILTERING_ONLY:
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
   BuildMultiScorer(walrus);
   ResolvePostmortemType();
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
   DetectScorerGoals();
   InitDeck();
   ui.AdaptMiniRows(this);
   if (config.IsInitFailed()) {
      owl.Show("Failed on semantic-dependent init\n");
      return false;
   }

   return StartOscar();
}
