/************************************************************
* Walrus project                                        2023
* Config reader
*
* TODO: https://docs.github.com/en/copilot/how-tos/configure-custom-instructions/add-repository-instructions
* Copilot Chat          Repository-wide instructions (using the .github/copilot-instructions.md file).
* Copilot coding agent  Repository-wide instructions, path-specific instructions, and agent instructions (using AGENTS.md, CLAUDE.md or GEMINI.md files).
* Copilot code review   Repository-wide instructions and path-specific instructions (using .github/instructions/../NAME.instructions.md files).
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
char* WaConfig::Keywords::Scale = "SCALE: ";
char* WaConfig::Keywords::Debug = "DEBUG: ";
char* WaConfig::Keywords::ShowOnAdded = "SHOW BOARD ON ADDED";
char* WaConfig::Keywords::ShowComparisons = "SHOW COMPARISONS";
char* WaConfig::Keywords::ShowOnReconstructed = "SHOW RECONSTRUCTED";
char* WaConfig::Keywords::VerboseCompile = "VERBOSE COMPILING";
char* WaConfig::Keywords::VerboseMemory = "VERBOSE MEMORY";
char* WaConfig::Keywords::PostmHCP = "HCP";
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
         solve.opMode = desc.val;
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
   strcat(filters.sourceCode, line);
   filters.sizeSourceCode = strlen(filters.sourceCode) + 1;
   if (filters.sizeSourceCode > sizeof(filters.sourceCode)) {
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
   if (IsStartsWith(line, key.VerboseMemory)) {
      dbg.verboseMemory = true;
   }
   if (IsStartsWith(line, key.ShowComparisons)) {
      dbg.verboseComparisons = true;
   }
}

void WaConfig::ReadScaleSetting(char* line)
{
   if (IsStartsWith(line, "MAX")) {
      solve.aimTaskCount = MAX_TASKS_TO_SOLVE;
      return;
   }

   if (IsStartsWith(line, "MIN")) {
      solve.aimTaskCount = 100;
      return;
   }

   auto kilos = atoi(line);
   if (kilos > 0) {
      solve.aimTaskCount = __min(kilos * 1000, MAX_TASKS_TO_SOLVE); 
      return;
   }

   printf("Warning: cannot read task scale from line: %s", line);
}

void WaConfig::ReadHandPBN(const char* line)
{
   strcpy(txt.taskHandPBN, "[");
   SAFE_ADD_BY_LINE(txt.taskHandPBN);
   SAFE_ADD(txt.taskHandPBN, "]");
}

void WaConfig::ReadPrimaScorer(const char* line)
{
   SAFE_STR_BY_LINE(txt.primaScorerCode);
   FillShortScorer(txt.primaScorerCode, txt.primaShort);
}

void WaConfig::ReadSecundaScorer(const char* line)
{
   SAFE_STR_BY_LINE(txt.secundaScorerCode);
   FillShortScorer(txt.secundaScorerCode, txt.secundaShort);
   solve.shouldSolveTwice = true;
}

void WaConfig::FillShortScorer(const char* from, char* to)
{
   if (strlen(from) > 4) {
      to[0] = from[1];
      to[1] = from[2];
      to[2] = 0;
   }
}

bool WaConfig::RecognizePostmType(const char* token)
{
   if (IsStartsWith(token, key.PostmHCP)) {
      if (solve.shouldSolveTwice) {
         postm.Type = WPM_A_TO_B;
      } else {
         postm.Type = WPM_HCP_SINGLE_SCORER;
      }
      return true;
   }

   if (IsStartsWith(token, "LEAD")) {
      postm.Type = WPM_OPENING_LEADS;
      return true;
   }

   if (IsStartsWith(token, "SUIT")) {
      postm.Type = WPM_SUIT;
      return true;
   }

   if (IsStartsWith(token, "AUTO")) {
      postm.Type = WPM_AUTO;
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
         case 0:// name
            RecognizePostmType(token);
            break;

         case 1:// 1st arg
            postm.minHCP = atoi(token);
            break;

         case 2:// 2nd arg
            postm.maxHCP = atoi(token);
            break;
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
      printf("Error: A short PBN notation is expected as leads. Example: like A.5.T.2\n Your line is: %s", line);
      MarkFail();
      return;
   }

   // convert
   solve.leads.S = IsCard(line[0]);
   solve.leads.H = IsCard(line[2]);
   solve.leads.D = IsCard(line[4]);
   solve.leads.C = IsCard(line[6]);
}

void WaConfig::AnnounceTask()
{
   if (IsInitFailed()) {
      return;
   }

   owl.Show("%s : %s", txt.nameTask, txt.titleBrief);
   owl.Show("Fixed hand is %s\n", txt.taskHandPBN);

   if (txt.primaScorerCode[0]) {
      if (txt.secundaScorerCode[0]) {
         owl.Show("Scorers to use are: %s; %s\n", txt.primaScorerCode, txt.secundaScorerCode);
      } else {
         owl.Show("Scorer to use is %s\n", txt.primaScorerCode);
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
   else if (strlen(line) > 2) {
      SAFE_ADD(txt.titleBrief, line);
      // TODO: when line starts with  "//" we may consider it a comment and skip it
   }

   return S_IN_TASK;
}

EConfigReaderState WaConfig::FSM_GoInsideTask(char* line)
{
   line += strlen(txt.nameTask) + 1;
   strcpy_s(txt.titleBrief, sizeof(txt.titleBrief), line);

   solve.opMode = OPMODE_FIXED_TASK;

   return S_IN_TASK;
}

EConfigReaderState WaConfig::FSM_Go2WaitTask(char* line)
{
   line += strlen(key.TName);
   SAFE_STR_BY_LINE(txt.nameTask);

   return S_WAIT_TASK;
}

void WaConfig::ReadTask(Walrus *walrus)
{
   // drop goals
   prim.goal = 0;
   secondary.goal = 0;

   // ensure we have a file
   const char* fname = txt.namesBase.StartFrom;
   printf("Reading config from: %s\n", fname);
   FILE* stream;
   if (fopen_s(&stream, fname, "r")) {// non-zero => failed to open
      return;
   }

   // prepare
   char line[128];
   txt.titleBrief[0] = 0;
   txt.primaScorerCode[0] = 0;
   txt.taskHandPBN[0] = 0;
   filters.sourceCode[0] = 0;

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
            if (IsStartsWith(line, txt.nameTask)) {
               fsm = FSM_GoInsideTask(line);
            } 
            break;
         }

         case S_IN_TASK: fsm = FSM_DoTaskState(line);    break;
         case S_FILTERS: fsm = FSM_DoFiltersState(line); break;
      }

      KEYWORD_CALS(Debug, ReadDebugSetting)
      KEYWORD_CALL(Scale, ReadScaleSetting)
   }

   // cleanup
   fclose(stream);

   // ensure we've visited some task
   if (txt.nameTask[0] && (solve.opMode == OPMODE_NONE)) {
      printf("Error: Task '%s' not found in the config file\n", txt.nameTask);
      MarkFail();
   }
}

void WaConfig::ResolvePostmortemType(Walrus* walrus)
{
   // failed before
   if (IsInitFailed()) {
      return;
   }

   // always read board 
   postm.hcpFixedHand = walrus->ParsePbnDeal();

   // pm none is ok
   if (postm.Type == WPM_NONE) {
      return;
   }

   // announce; resolve auto 
   printf("Postmortem type: ");
   if (postm.Type == WPM_HCP_SINGLE_SCORER ||
       postm.Type == WPM_A_TO_B) {
      printf("(deprecated; consider using AUTO) ");
   } else if (postm.Type == WPM_AUTO) {
      printf("Auto --> ");
      RecognizePostmType(key.PostmHCP);
      if (!filters.FindHCPRange(SOUTH, postm.minHCP, postm.maxHCP)) {
         printf("Error: missing PointsRange for SOUTH\n");
         MarkFail();
         return;
      }
      postm.minHCP += postm.hcpFixedHand.total;
      postm.maxHCP += postm.hcpFixedHand.total;
   }

   // may detect controls
   if (postm.minHCP) {
      if (postm.minHCP == postm.maxHCP) {
         postm.minControls = (postm.minHCP * 4) / 10 - 6;
      }
   }

   // final types
   bool checkRange = false;
   bool checkLeads = false;
   switch (postm.Type) {
      case WPM_HCP_SINGLE_SCORER:
         if (postm.minControls) {
            postm.Type = WPM_CONTROLS;
            printf("Controls for %d hcp\n", postm.minHCP);
         } else {
            printf("HCP single scorer: %d to %d\n", postm.minHCP, postm.maxHCP);
         }
         strcpy(config.txt.freqTitleFormat, "TRICKS FREQUENCY FOR %d HCP");
         checkRange = true;
         break;

      case WPM_A_TO_B:
         printf("A to B comparator with HCP %d to %d\n", postm.minHCP, postm.maxHCP);
         strcpy(config.txt.freqTitleFormat, "COMPARISON RESULTS FOR %d HCP");
         checkRange = true;
         break;

      case WPM_OPENING_LEADS:// lead task should have lead cards specified
         printf("Opening Leads\n");
         strcpy(config.txt.freqTitleFormat, "TRICKS FREQUENCY FOR %s LEAD");
         checkLeads = true;
         break;

      case WPM_SUIT:
         printf("Suit\n");
         strcpy(config.txt.freqTitleFormat, "TRICKS FREQUENCY FOR HCP in a suit (which?)");
         break;

      default:
         printf("unrecognized pm: %d\n", postm.Type);
         MarkFail();
         break;
   }

   // final checks
   if (checkRange) {
      if ((postm.minHCP < 0) || (postm.maxHCP < 0) || (postm.minHCP > postm.maxHCP)) {
         printf("Error: invalid HCP range in postmortem: min=%d, max=%d\n", postm.minHCP, postm.maxHCP);
         MarkFail();
      }
   }
   if (checkLeads) {
      auto sum = solve.leads.S + solve.leads.H + solve.leads.D + solve.leads.C;
      if (!sum) {
         printf("Error: '%s' line is missing or in a wrong format.\n", key.Leads);
         MarkFail();
      }
   }
}

