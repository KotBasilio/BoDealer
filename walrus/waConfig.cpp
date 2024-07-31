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

bool IsStartsWith(const char *str, const char *prefix) {
   size_t lenPrefix = strlen(prefix);
   size_t lenStr = strlen(str);

   if (lenStr < lenPrefix) {
      return false;
   }

   return strncmp(str, prefix, lenPrefix) == 0;
}

char* WaConfig::Keywords::OpMode = "OPMODE: ";
char* WaConfig::Keywords::Hand = "HAND: ";
char* WaConfig::Keywords::TName = "TASK NAME:";
char* WaConfig::Keywords::Filters = "FILTERS:";
char* WaConfig::Keywords::TEnd = "TASK END";

EConfigReaderState WaConfig::FSM_DoFiltersState(char *line)
{
   // ends
   if (IsStartsWith(line, "ENDF")) {
      return S_IN_TASK;
   }
   if (IsStartsWith(line, key.TEnd)) {
      return S_IDLE;
   }

   // add and control
   strcat(sourceCodeFilters, line);
   sizeSourceCode = strlen(sourceCodeFilters) + 1;
   if (sizeSourceCode > sizeof(sourceCodeFilters)) {
      printf("Error: exceeded source code size. Exiting.\n");
      PLATFORM_GETCH();
      exit(0);
   }

   return S_FILTERS;
}

void WaConfig::ReadHandPBN(const char* line)
{
   strcpy(taskHandPBN, "[");
   strcat_s(taskHandPBN, sizeof(taskHandPBN), line);
   taskHandPBN[strlen(taskHandPBN) - 1] = 0;
   strcat_s(taskHandPBN, sizeof(taskHandPBN), "]");
}

EConfigReaderState WaConfig::FSM_DoTaskState(char* line)
{
   if (IsStartsWith(line, key.Filters)) {
      owl.Show("%s : %s", nameTask, titleBrief);
      owl.Show("Fixed hand is %s\n", taskHandPBN);
      return S_FILTERS; 
   } 
   
   if (IsStartsWith(line, key.TEnd)) {
      return S_IDLE;
   } 
   
   if (IsStartsWith(line, key.OpMode)) {
      ChangeOpMode(line + strlen(key.OpMode));
   } else if (IsStartsWith(line, key.Hand)) {
      ReadHandPBN(line + strlen(key.Hand));
   } else if (strlen(line) > 2) {
      strcat_s(titleBrief, sizeof(titleBrief), line);
   }

   return S_IN_TASK;
}

void WaConfig::ReadTask(Walrus *walrus)
{
   // drop goals
   primGoal = 0;
   secGoal = 0;

   // ensure we have a file
   const char* fname = namesBase.StartFrom;
   printf("Reading config from: %s\n", fname);
   FILE* stream;
   if (fopen_s(&stream, fname, "r")) {// non-zero => failed to open
      return;
   }

   // prepare
   char line[128];
   titleBrief[0] = 0;
   taskHandPBN[0] = 0;
   sourceCodeFilters[0] = 0;

   // fsm on all lines
   EConfigReaderState fsm = S_IDLE;
   while (!feof(stream)) {
      if (!fgets(line, sizeof(line), stream)) {
         break;
      }
      //printf(line);

      switch (fsm) {
         case S_IDLE: {
            if (IsStartsWith(line, key.TName)) {
               strcpy_s(nameTask, sizeof(nameTask), line + strlen(key.TName));
               fsm = S_WAIT_TASK;
               nameTask[strlen(nameTask) - 1] = 0;
            }
            break;
         }

         case S_WAIT_TASK: {
            if (IsStartsWith(line, nameTask)) {
               fsm = S_IN_TASK;
               strcpy_s(titleBrief, sizeof(titleBrief), line + strlen(nameTask) + 1);
            } 
            break;
         }

         case S_IN_TASK: {
            fsm = FSM_DoTaskState(line);
            break;
         }
         case S_FILTERS:  {
            fsm = FSM_DoFiltersState(line);
            break;
         }

         case S_GOALS:
         default:
            break;
      }

   }
   fclose(stream);

   BuildNewFilters(walrus);
}

void WaConfig::BuildNewFilters(Walrus *walrus)
{
   if (!sizeSourceCode) {
      printf("No filters are found in the config.\n");
      return;
   }
   printf("A filters source code is found in the config. Passing to compiler, size is %llu of %llu.\n", 
      sizeSourceCode, sizeof(sourceCodeFilters));

   if (!walrus->sem.Compile(sourceCodeFilters, sizeSourceCode, filtersLoaded)) {
      printf("Config ERROR: Failed to compile filters.\n");
      return;
   }

   if (!walrus->sem.MiniLink(filtersLoaded)) {
      printf("Config ERROR: Failed to link filters.\n");
      return;
   }

   printf("Success on filters compiling and linking.\n");
}

struct OpModeDesc {
   WA_OPERATION_MODE val;
   const char* key;
};

#define DESCRIBE_OPMODE(NAME)     {OPMODE_##NAME, #NAME}

static OpModeDesc OpModes[] =
{
   DESCRIBE_OPMODE(FIXED_TASK),
   DESCRIBE_OPMODE(SEMI_STRAY),
   DESCRIBE_OPMODE(STRAY),
   DESCRIBE_OPMODE(DEMO_STATISTICS)
};

void WaConfig::ChangeOpMode(const char* line)
{
   for (auto& desc : OpModes) {
      if (IsStartsWith(line, desc.key)) {
         opMode = desc.val;
         printf("Recognized op-mode: %s", line);
      }
   }
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
      ScoreWithPrimary(tr);
      AddScorerValues(tail);
   }
   owl.Show("%s\n", tail);

   // secondary
   bool shouldSkipSecunda = cumulScore.secunda.IsEmpty() || (sem.onDepPrimaryScoring == &CumulativeScore::BiddingLevel);
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
      ScoreWithPrimary(tr);
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
         ScoreWithSecondary(tr);
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
   config.ReadTask(this);

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

