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

#define KEYWORD_CALS(KEY, FUNC)  if (IsStartsWith(line, key.KEY)) { FUNC(line + strlen(key.KEY)); }
#define KEYWORD_CALL(KEY, FUNC)  else KEYWORD_CALS(KEY, FUNC)

char* WaConfig::Keywords::OpMode = "OPMODE: ";
char* WaConfig::Keywords::Hand = "HAND: ";
char* WaConfig::Keywords::Leads = "LEAD CARDS: ";
char* WaConfig::Keywords::TName = "TASK NAME:";
char* WaConfig::Keywords::Prima = "PRIMARY SCORER: ";
char* WaConfig::Keywords::Secunda = "SECONDARY SCORER: ";
char* WaConfig::Keywords::Postmortem = "POSTMORTEM: ";
char* WaConfig::Keywords::Filters = "FILTERS:";
char* WaConfig::Keywords::TEnd = "--------";
char* WaConfig::Keywords::Debug = "DEBUG: ";
char* WaConfig::Keywords::ShowOnAdded = "SHOW BOARD ON ADDED";
char* WaConfig::Keywords::ShowOnReconstructed = "SHOW RECONSTRUCTED";
char* WaConfig::Keywords::VerboseCompile = "VERBOSE COMPILING";
char* WaConfig::Keywords::Delimiters = " ,.!:;[]()+-\n";

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
void WaConfig::ReadDebugSetting(char* line)
{
   if (IsStartsWith(line, key.ShowOnAdded)) {
      dbg.viewBoardOnAdd = true;
   }
   if (IsStartsWith(line, key.ShowOnReconstructed)) {
      dbg.viewBoardOnReconstruct = true;
   }
   if (IsStartsWith(line, key.VerboseCompile)) {
      dbg.verboseCompile = true;
   }
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

void WaConfig::ReadSecundaScorer(const char* line)
{
   SAFE_STR_BY_LINE(secundaScorerCode);
}

bool WaConfig::RecognizePostmType(const char* token)
{
   if (IsStartsWith(token, "HCP")) {
      postm.reportType = WPM_HCP;
      strcpy(config.postm.freqTitleFormat, "TRICKS FREQUENCY FOR %d HCP");
      return true;
   }

   if (IsStartsWith(token, "LEAD")) {
      postm.reportType = WPM_OPENING_LEADS;
      strcpy(config.postm.freqTitleFormat, "TRICKS FREQUENCY FOR %s LEAD");
      return true;
   }

   printf("Error: unrecognized postmortem type '%s'\n", token);
   MarkFail();
   return false;
}

void WaConfig::ReadPostmortemParams(char* line)
{
   // parse the line
   int idx = 0;
   for (char* token = std::strtok(line, key.Delimiters);
        token && isInitSuccess;
        token = std::strtok(nullptr, key.Delimiters), idx++) {
      switch (idx) {
         case 0:
            RecognizePostmType(token);
            break;

         case 1:
            postm.minHCP = atoi(token);
            break;

         case 2:
            postm.maxHCP = atoi(token);
            break;
      }
   }

   // may detect controls
   if (postm.minHCP) {
      if (postm.minHCP == postm.maxHCP) {
         postm.minControls = (postm.minHCP * 4) / 10 - 6;
      }
   }
}

extern int IsCard(const char cardChar);

void WaConfig::ReadLeadCards(const char* line)
{
   // check format
   bool ok = (strlen(line) >= 7) && 
      line[1] == '.' &&
      line[3] == '.' &&
      line[5] == '.';
   if (!ok) {
      printf("Error: A short PBN notation (like A.5.T.2) is expected as leads. You line: %s", line);
      MarkFail();
      return;
   }

   // convert
   leads.S = IsCard(line[0]);
   leads.H = IsCard(line[2]);
   leads.D = IsCard(line[4]);
   leads.C = IsCard(line[6]);
}

void WaConfig::AnnounceTask()
{
   if (IsInitFailed()) {
      return;
   }

   owl.Show("%s : %s", nameTask, titleBrief);
   owl.Show("Fixed hand is %s\n", taskHandPBN);

   if (primaScorerCode[0]) {
      if (secundaScorerCode[0]) {
         owl.Show("Scorers to use are: %s; %s\n", primaScorerCode, secundaScorerCode);
      } else {
         owl.Show("Scorer to use is %s\n", primaScorerCode);
      }
   }
}


EConfigReaderState WaConfig::FSM_DoTaskState(char* line)
{
   if (IsStartsWith(line, key.Filters)) {
      AnnounceTask();
      return S_FILTERS; 
   } 
   
   if (IsStartsWith(line, key.TEnd)) {
      return S_IDLE;
   } 
   
   KEYWORD_CALS(OpMode,      ChangeOpMode)
   KEYWORD_CALL(Hand,        ReadHandPBN)
   KEYWORD_CALL(Leads,       ReadLeadCards)
   KEYWORD_CALL(Prima,       ReadPrimaScorer)
   KEYWORD_CALL(Secunda,     ReadSecundaScorer)
   KEYWORD_CALL(Postmortem,  ReadPostmortemParams)
   KEYWORD_CALL(Debug,       ReadDebugSetting)
   else if (strlen(line) > 2) {
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
   prim.goal = 0;
   secondary.goal = 0;

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
            KEYWORD_CALL(Debug, ReadDebugSetting)
            break;
         }

         case S_IN_TASK: fsm = FSM_DoTaskState(line);    break;
         case S_FILTERS: fsm = FSM_DoFiltersState(line); break;
      }
   }

   // cleanup
   fclose(stream);

   // ensure we've visited some task
   if (nameTask[0] && (opMode == OPMODE_NONE)) {
      printf("Error: Task '%s' not found in the config file\n", nameTask);
      MarkFail();
   }

   // lead task should have leads
   if (postm.Is(WPM_OPENING_LEADS)) {
      auto sum = leads.S + leads.H + leads.D + leads.C;
      if (!sum) {
         printf("Error: '%s' line is missing or in a wrong format.\n", key.Leads);
         MarkFail();
      }
   }

}

