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
      //printf("%s", line);
   }

   fclose(stream);

   //PLATFORM_GETCH();
}

void Walrus::DetectGoals()
{
   char tail[128], chunk[20];
   DdsTricks tr;

   // primary
   cfgTask.primGoal = PokeScorerForTricks();
   printf("Primary scorer (%s, %d tr):", ui.declTrump, cfgTask.primGoal);
   strcpy(tail, "  / ");
   for (tr.plainScore = 7; tr.plainScore <= 13 ; tr.plainScore++) {
      cumulScore.bidGame = 0;
      cumulScore.partscore = 0;
      (this->*sem.onScoring)(tr);
      printf(" %lld", cumulScore.bidGame);
      if (cumulScore.partscore) {
         sprintf(chunk, " %lld", cumulScore.partscore);
         strcat(tail, chunk);
      }
   }
   printf("%s\n", tail);

   // secondary
   if (sem.solveSecondTime == &Walrus::SolveSecondTime) {
      cfgTask.otherGoal = PokeOtherScorer();
      printf("Secondary scorer (%s, %d tr):", ui.theirTrump, cfgTask.otherGoal);
      strcpy(tail, "  / ");
      for (tr.plainScore = 7; tr.plainScore <= 13; tr.plainScore++) {
         cumulScore.oppContract = 0;
         cumulScore.oppCtrDoubled = 0;
         (this->*sem.onSolvedTwice)(tr);
         printf(" %lld", - cumulScore.oppContract);
         if (cumulScore.oppCtrDoubled) {
            sprintf(chunk, " %lld", - cumulScore.oppCtrDoubled);
            strcat(tail, chunk);
         }
      }
      printf("%s\n", tail);
   }

   // all this poking left some score
   CumulativeScore zeroes;
   cumulScore = zeroes;
   //PLATFORM_GETCH();
}


bool Walrus::InitByConfig()
{
   // may read something
   cfgTask.ReadStart();

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

