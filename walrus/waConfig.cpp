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

void WaConfig::ReadStart(Walrus *walrus)
{
   // goals
   primGoal = 0;
   secGoal = 0;

   if (LoadFiltersSource()) {
      BuildNewFilters(walrus);
   }

   //PLATFORM_GETCH();
}

void WaConfig::BuildNewFilters(Walrus *walrus)
{
   // try
   if (walrus->sem.Compile(sourceCodeFilters, sizeSourceCode, filtersLoaded)) {
      walrus->sem.MiniLink(filtersLoaded);
   } else {
      printf("Config ERROR: Failed to compile filters.\n");
   }
}

bool IsStartsWith(const char *str, const char *prefix) {
   size_t lenPrefix = strlen(prefix);
   size_t lenStr = strlen(str);

   if (lenStr < lenPrefix) {
      return false;
   }

   return strncmp(str, prefix, lenPrefix) == 0;
}

enum EConfigReaderState {
   S_IDLE,
   S_FILTERS,
   S_BIDDING,
   S_GOALS
};

bool WaConfig::LoadFiltersSource()
{
   const char* fname = namesBase.StartFrom;
   //printf("Reading config from: %s\n", fname);
   //PLATFORM_GETCH();

   FILE* stream;
   if (fopen_s(&stream, fname, "r")) {// non-zero => failed to open
      return false;
   }

   sourceCodeFilters[0] = 0;
   bool copying = false;
   while (!feof(stream)) {
      char line[100];
      if (!fgets(line, sizeof(line), stream)) {
         break;
      }
      printf(line);

      // only one non-idle state of FSM so far
      if (copying) {
         if (IsStartsWith(line, "ENDF")) {
            copying = false;
         } else {
            strcat(sourceCodeFilters, line);
         }
      } else {
         if (IsStartsWith(line, "FILTERS:")) {
            copying = true;
         }
      }
      sizeSourceCode = strlen(sourceCodeFilters) + 1;
      if (sizeSourceCode > WA_SOURCE_CODE_BUF) {
         printf("Error: exceeded source code size. Exiting\n");
         PLATFORM_GETCH();
         exit(0);
      }
   }

   fclose(stream);

   printf("Code size: %llu of %llu\n", sizeSourceCode, WA_SOURCE_CODE_BUF);
   return true;
}

void Walrus::DetectGoals()
{
   // detect old scorers
   if (cumulScore.prima.IsEmpty() && cumulScore.secunda.IsEmpty()) {
      DeprDetectGoals();
      return;
   }
   config.SetupOtherContract();

   // display primary scorer
   DdsTricks tr;
   char tail[128];
   CumulativeScore zeroes(cumulScore);
   owl.Show("Primary scorer (%s, %d tr):", ui.declTrump, config.primGoal);
   strcpy(tail, "  / ");
   for (tr.plainScore = 7; tr.plainScore <= 13 ; tr.plainScore++) {
      cumulScore = zeroes;
      (cumulScore.*sem.onScoring)(tr.plainScore);
      AddScorerValues(tail);
   }
   owl.Show("%s\n", tail);

   // secondary
   bool shouldSkipSecunda = cumulScore.secunda.IsEmpty() || (sem.onScoring == &CumulativeScore::BiddingLevel);
   if (!shouldSkipSecunda) {
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
}

void Walrus::AddScorerValues(char* tail)
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
      return;
   }

   if (cumulScore.bidPartscore) {
      sprintf(chunk, " %lld", cumulScore.bidPartscore);
      strcat(tail, chunk);
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
   if (hasSecondaryScorer) {
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

bool Walrus::InitByConfig()
{
   // may read something
   config.ReadStart(this);

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

