/************************************************************
* WALRUS config and other input/output                   2024
*
************************************************************/

// file names
#define START_FROM_FNAME "start_from.txt"
#define COMMAND_FNAME    "in_cmd.txt"
#define OUT_FNAME        "stm%d%d%d%d%d%d%d%d%d%d.txt"
#define PROGRESS_FNAME   "waProgress.txt"
const int FNAME_SIZE = 128 + 64;
struct waFileNames {
   char StartFrom[FNAME_SIZE];
   char Command [FNAME_SIZE];
   char Progress[FNAME_SIZE];
   char Solution[FNAME_SIZE];
   void Build();
};

#define DDS_FULL_LINE 76
#define DDS_HAND_OFFSET 12
#define DDS_HAND_LINES 12

struct MiniUI {
   bool  exitRequested;
   bool  reportRequested;
   bool  advancedStatistics, allStatGraphs;
   bool  firstAutoShow;
   bool  shownDashes;
   int   irGoal, irFly;
   s64   primaBetterBy;
   int   farCol;
   int   indent;
   MiniUI();
   void RecognizeCommands(int inchar);
   void VoidAdded(twContext* lay) {}
   void DisplayBoard(twContext* lay);
   void WaitAnyKey();
   void FillMiniRows();
   void AdaptMiniRows(Walrus* wal);
   void Run();
};

enum WA_POSTM_TYPE {
   WPM_NONE = 0,
   WPM_AUTO,
   WPM_HCP_SINGLE_SCORER,
   WPM_A_TO_B,
   WPM_CONTROLS,
   WPM_SUIT,
   WPM_OPENING_LEADS,
};

enum WA_OPERATION_MODE {
   OPMODE_NONE = 0,
   OPMODE_FIXED_TASK,
   OPMODE_SEMI_STRAY,
   OPMODE_STRAY,
   OPMODE_DEMO_STATISTICS,
};

enum EConfigReaderState {
   S_IDLE = 0,
   S_WAIT_TASK,
   S_IN_TASK,
   S_FILTERS,
   S_GOALS
};

constexpr size_t WA_SCORER_CODE_LEN = 32;
constexpr size_t WA_SHORT_SCORER_LEN = 4;
constexpr size_t WA_TASK_BRIEF = 1024;
constexpr size_t WA_TASK_NANE_LEN = 64;
constexpr size_t WA_SECONDARY_LNAME_LEN = WA_TASK_NANE_LEN * 2;
constexpr size_t WA_HAND_LEN = 30;
constexpr size_t WA_TXT_SEAT_SUIT = 10;
constexpr size_t WA_SOURCE_CODE_BUF = 2 * 1024;

struct WaConfig {
   WaConfig();
   bool isInitSuccess = true;

   // various texts
   struct Txt {
      waFileNames namesBase;
      char nameTask[WA_TASK_NANE_LEN];
      char titleBrief[WA_TASK_BRIEF];   // a title and a brief
      char primaScorerCode[WA_SCORER_CODE_LEN];   // our main action in linear scorer format
      char secundaScorerCode[WA_SCORER_CODE_LEN]; // second action
      char primaShort[WA_SHORT_SCORER_LEN];
      char secundaShort[WA_SHORT_SCORER_LEN];
      char taskHandPBN[WA_HAND_LEN];
      char secLongName[WA_SECONDARY_LNAME_LEN];
      char freqTitleFormat[WA_TASK_NANE_LEN];
   } txt;

   // filters compiling
   struct Filters {
      char   sourceCode[WA_SOURCE_CODE_BUF];
      size_t sizeSourceCode = 0;
      std::vector<MicroFilter> compiled;
      Filters();
      bool FindHCPRange(int hand, int &from, int &to);
   } filters;

   // deck
   struct Deck {
      uint cardsCount = SOURCE_CARDS_COUNT;
      uint cardsRemoved = 0;
   } deck;

   // solving
   struct Solving {
      uint aimTaskCount = 20*1000;
      WA_OPERATION_MODE opMode = OPMODE_NONE;
      int ddsSol = 1;
      struct Leads { // cards to lead for WPM_OPENING_LEADS
         int S=0, H=0, D=0, C=0;
      } leads;
      bool shouldSolveTwice = false;
   } solve;

   // contracts and its text representation
   struct Contract {
      int   goal;  // goal tricks
      int   trump;
      int   by;    // declarer
      int   first; // attacker

      char txtTrump[WA_TXT_SEAT_SUIT];
      char txtBy[WA_TXT_SEAT_SUIT];
      char txtAttacker[WA_TXT_SEAT_SUIT];
      Contract();
      void Init(const CumulativeScore::LineScorer& scorer);
   };
   Contract prim;      // our primary contract
   Contract secondary; // either our secondary contract or their contract

   // adding extra marks aka post-mortem 
   struct Postmortem {
      WA_POSTM_TYPE  Type;
      int            minHCP, maxHCP;
      int            minControls;
      twlHCP         hcpFixedHand;
      Postmortem();
      bool Is(WA_POSTM_TYPE t) { return (t == Type); }
      int  FactorFromRow(int idx);
   } postm;
   int   postmSuit, postmHand; 

   // display options
   struct Output {
      bool showHugeMatch = false;
      bool showOurOther = false;
      bool showOppResults = false;
      bool oppsOnlyDoubled = false;
      bool oppsOnlyPassed = false;
      bool seekDecisionCompete = false;
      bool showMagicFly = false;
      bool showPercentages = false;
      int  rowPercentage = 0;
      bool DisplayingOtherContract() { return showOppResults || showOurOther || seekDecisionCompete; }
   } io;

   // debug options
   struct Debug {
      bool viewBoardOnAdd = false;
      bool viewBoardOnReconstruct = false;
      bool verboseCompile = false;
      bool verboseMemory = false;
      bool verboseComparisons = false;
   } dbg;

   bool OrdinaryRead(Walrus* walrus);
   void BuildNewFilters(class Walrus *walrus);
   void SetupSeatsAndTrumps(const struct CumulativeScore &cs);
   void MarkFail() { isInitSuccess = false; }
   bool IsInitFailed() { return !isInitSuccess; }
private:

   void AnnounceTask();
   void ChangeOpMode(const char *line);
   void ReadTask(class Walrus *walrus);
   void InitCardsCount();
   void ResolvePostmortemType(Walrus* walrus);
   void SetupOutputOptions();
   void ReadHandPBN(const char *line);
   void ReadLeadCards(const char *line);
   void ReadPrimaScorer(const char *line);
   void ReadSecundaScorer(const char *line);
   void FillShortScorer(const char *from, char *to);
   void ReadPostmortemParams(char *line);
   void ReadDebugSetting(char *line);
   void ReadScaleSetting(char *line);
   EConfigReaderState FSM_DoFiltersState(char* line);
   EConfigReaderState FSM_DoTaskState(char* line);
   EConfigReaderState FSM_Go2WaitTask(char* line);
   EConfigReaderState FSM_GoInsideTask(char* line);
   bool RecognizePostmType(const char* token);

   // keywords used in parsing
   struct Keywords {
      static char* Debug;
      static char* Delimiters;
      static char* Filters;
      static char* Hand;
      static char* Leads;
      static char* OpMode;
      static char* Postmortem;
      static char* Prima, *Secunda;
      static char* Scale;
      static char* ShowComparisons;
      static char* ShowOnAdded;
      static char* ShowOnReconstructed;
      static char* TEnd;
      static char* TName;
      static char* VerboseCompile;
      static char* VerboseMemory;
      static char* PostmHCP;
   } key;
};

extern WaConfig config;

