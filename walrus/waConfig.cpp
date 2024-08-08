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
   config.SetupSeatsAndTrumps();

   // display primary scorer
   DdsTricks tr;
   char tail[128];
   CumulativeScore zeroes(cumulScore);
   owl.Show("Primary scorer (%s, %d tr):", config.declTrump, config.primGoal);
   strcpy(tail, "  / ");
   for (tr.plainScore = 7; tr.plainScore <= 13 ; tr.plainScore++) {
      cumulScore = zeroes;
      ScoreWithPrimary(tr);
      AddScorerValues(tail);
   }
   owl.Show("%s\n", tail);

   // secondary
   bool shouldSkipSecunda = cumulScore.secunda.IsEmpty() || (sem.onSinglePrimary == &CumulativeScore::BiddingLevel);
   if (!shouldSkipSecunda) {
      owl.Show("Contract-B scorer (%s, %d tr):", config.theirTrump, config.secGoal);
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

   #ifdef SEEK_OPENING_LEAD
   if (cumulScore.ideal) {
      owl.Show(" %lld", cumulScore.ideal);
      return;
   }
   #endif SEEK_OPENING_LEAD

   if (cumulScore.oppContract) {
      owl.Show(" %lld", -cumulScore.oppContract);
      if (cumulScore.oppCtrDoubled) {
         sprintf(chunk, " %lld", -cumulScore.oppCtrDoubled);
         strcat(tail, chunk);
      }
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

   sem.MiniLinkFilters();

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
   if (config.IsInitFailed()) {
      owl.Show("Failed on semantic-dependent init\n");
      return false;
   }

   return StartOscar();
}

