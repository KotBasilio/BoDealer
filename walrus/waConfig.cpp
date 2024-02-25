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
   otherGoal = 0;

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
   DdsTricks tr;
   char tail[128];
   CumulativeScore zeroes;
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
      config.otherGoal = PokeOtherScorer();
      ui.SetupOtherContract();
      owl.Show("Contract-B scorer (%s, %d tr):", ui.theirTrump, config.otherGoal);
      strcpy(tail, "  / ");
      for (tr.plainScore = 7; tr.plainScore <= 13; tr.plainScore++) {
         cumulScore = zeroes;
         (this->*sem.onSolvedTwice)(tr);
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

   owl.Show(" %lld", cumulScore.bidGame);
   if (cumulScore.partscore) {
      sprintf(chunk, " %lld", cumulScore.partscore);
      strcat(tail, chunk);
   } else if (cumulScore.bidSlam) {
      sprintf(chunk, " %lld", cumulScore.bidSlam);
      strcat(tail, chunk);
   }
}


bool Walrus::InitByConfig()
{
   // may read something
   config.ReadStart();

   // prepare, basing on config
   FillSemantic();
   sem.MiniLink();
   InitDeck();
   memset(progress.hitsCount, 0, sizeof(progress.hitsCount));
   shuf.SeedRand();
   InitMiniUI();
   DetectGoals();

   return StartOscar();
}

