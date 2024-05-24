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
      const char *key = "Dealer";
      char* in = strstr(buf, key);
      if (in) {
         in += strlen(key);
         in[1] = 0;
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

void WaConfig::ReadStart()
{
   // goals
   primGoal = 0;
   secGoal = 0;

   const char* fname = namesBase.StartFrom;
   //printf("Reading config from: %s\n", fname);
   //PLATFORM_GETCH();

   FILE* stream;
   if (!fopen_s(&stream, fname, "r") == 0) {
      return;
   }

   while (!feof(stream)) {
      char line[100];
      if (!fgets(line, sizeof(line), stream)) {
         break;
      }
      //printf(line);
   }

   fclose(stream);

   //PLATFORM_GETCH();
}

void Walrus::DetectGoals()
{
   // detect old scorers
   if (cumulScore.prima.IsEmpty() && cumulScore.secunda.IsEmpty()) {
      DeprDetectGoals();
      return;
   }
}

void Walrus::DeprDetectGoals()
{
   DdsTricks tr;
   char tail[128];
   CumulativeScore zeroes(cumulScore);
   bool hasSecondaryScorer = (sem.solveSecondTime == &Walrus::SolveSecondTime);

   // primary
   config.primGoal = PokeScorerForTricks();
   owl.Show("%s scorer (%s, %d tr):"
      , hasSecondaryScorer ? "Contract-A" : "Primary"
      , ui.declTrump, config.primGoal);
   strcpy(tail, "  / ");
   for (tr.plainScore = 7; tr.plainScore <= 13 ; tr.plainScore++) {
      cumulScore = zeroes;
      (cumulScore.*sem.onScoring)(tr.plainScore);
      AddScorerValues(tail);
   }
   owl.Show("%s\n", tail);

   // secondary
   if (sem.solveSecondTime == &Walrus::SolveSecondTime) {
      config.secGoal = PokeOtherScorer();
      config.SetupOtherContract();
      owl.Show("Contract-B scorer (%s, %d tr):", config.theirTrump, config.secGoal);
      strcpy(tail, "  / ");
      for (tr.plainScore = 7; tr.plainScore <= 13; tr.plainScore++) {
         cumulScore = zeroes;
         (cumulScore.*sem.onSolvedTwice)(tr.plainScore);
         AddScorerValues(tail);
      }
      owl.Show("%s\n", tail);
   }

   // final cleanup
   cumulScore = zeroes;

   //PLATFORM_GETCH();
}

void Walrus::AddScorerValues(char *tail)
{
   char chunk[20];
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
   }
}

bool Walrus::InitByConfig()
{
   PrepareLinearScores();

   // may read something
   config.ReadStart();

   // semantic preparations
   FillSemantic();
   sem.MiniLinkFilters();
   if (sem.IsInitFailed()) {
      owl.Show("Failed to init semantics\n");
      return false;
   }

   // other preparations basing on config
   InitDeck();
   memset(progress.hitsCount, 0, sizeof(progress.hitsCount));
   shuf.SeedRand();
   InitMiniUI();
   DetectGoals();

   return StartOscar();
}

