/************************************************************
* Walrus project                                        2023
* Config reader
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include HEADER_CURSES
#include "walrus.h"

#define CUT_AT_TAIL(STR)     \
   STR[sizeof(STR) - 1] = 0; \
   STR[strlen(STR) - 1] = 0

#define SAFE_STR_BY_LINE(TOSTR)         \
   strncpy(TOSTR, line, sizeof(TOSTR)); \
   CUT_AT_TAIL(TOSTR)

#define SAFE_ADD_BY_LINE(TOSTR)                         \
   strncat(TOSTR, line, sizeof(TOSTR) - strlen(TOSTR)); \
   CUT_AT_TAIL(TOSTR)

#define SAFE_ADD(TOSTR, ADDITION) \
   strncat(TOSTR, ADDITION, sizeof(TOSTR) - strlen(TOSTR))

char* WaConfig::Keywords::OpMode = "OPMODE: ";
char* WaConfig::Keywords::Hand = "HAND: ";
char* WaConfig::Keywords::TName = "TASK NAME:";
char* WaConfig::Keywords::Prima = "PRIMARY SCORER: ";
char* WaConfig::Keywords::Secunda = "SECONDARY SCORER: ";
char* WaConfig::Keywords::Filters = "FILTERS:";
char* WaConfig::Keywords::TEnd = "--------";

static bool IsStartsWith(const char *str, const char *prefix) 
{
   size_t lenPrefix = strlen(prefix);
   size_t lenStr = strlen(str);

   if (lenStr < lenPrefix) {
      return false;
   }

   return strncmp(str, prefix, lenPrefix) == 0;
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
   SAFE_ADD_BY_LINE(taskHandPBN);
   SAFE_ADD(taskHandPBN, "]");
}

void WaConfig::ReadPrimaScorer(const char* line)
{
   SAFE_STR_BY_LINE(primaScorerCode);
}

EConfigReaderState WaConfig::FSM_DoTaskState(char* line)
{
   if (IsStartsWith(line, key.Filters)) {
      owl.Show("%s : %s", nameTask, titleBrief);
      owl.Show("Fixed hand is %s\n", taskHandPBN);
      if (primaScorerCode[0]) {
         owl.Show("Scorer to use is %s\n", primaScorerCode);
      }
      return S_FILTERS; 
   } 
   
   if (IsStartsWith(line, key.TEnd)) {
      return S_IDLE;
   } 
   
   if (IsStartsWith(line, key.OpMode)) {
      ChangeOpMode(line + strlen(key.OpMode));
   } else if (IsStartsWith(line, key.Hand)) {
      ReadHandPBN(line + strlen(key.Hand));
   } else if (IsStartsWith(line, key.Prima)) {
      ReadPrimaScorer(line + strlen(key.Prima));
   } else if (strlen(line) > 2) {
      SAFE_ADD(titleBrief, line);
   }

   return S_IN_TASK;
}

EConfigReaderState WaConfig::FSM_GoInsideTask(char* line)
{
   line += strlen(nameTask) + 1;
   strcpy_s(titleBrief, sizeof(titleBrief), line);

   opMode = OPMODE_FIXED_TASK;

   return S_IN_TASK;
}

EConfigReaderState WaConfig::FSM_Go2WaitTask(char* line)
{
   line += strlen(key.TName);
   SAFE_STR_BY_LINE(nameTask);

   return S_WAIT_TASK;
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
   primaScorerCode[0] = 0;
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
               fsm = FSM_Go2WaitTask(line);
            }
            break;
         }

         case S_WAIT_TASK: {
            if (IsStartsWith(line, nameTask)) {
               fsm = FSM_GoInsideTask(line);
            } 
            break;
         }

         case S_IN_TASK: fsm = FSM_DoTaskState(line);    break;
         case S_FILTERS: fsm = FSM_DoFiltersState(line); break;
      }
   }

   // ensure we were in task
   if (nameTask[0] && (opMode == OPMODE_NONE)) {
      printf("Error: Task '%s' not found in the config file\n", nameTask);
   }

   // cleanup
   fclose(stream);
}

