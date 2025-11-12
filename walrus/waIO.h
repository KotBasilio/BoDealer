/************************************************************
* WALRUS config and other input/output                   2024
*
************************************************************/

#include "WaSemMicro.h"

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

enum WA_TASK_TYPE {
   TTYPE_NONE = 0,
   // how many hands are involved
   TTYPE_FIXED_ONE_HAND,
   TTYPE_FOUR_HANDS_TASK,
   TTYPE_FILTERING_ONLY,
   // one hand variants
   TTYPE_ONE_SIDED_BIDDING_LEVEL = 10,
   TTYPE_ONE_SIDED_DENOMINATION,
   TTYPE_COMPETITIVE_GENERIC,
   TTYPE_SEEK_OPENING_LEAD
};

enum EConfigReaderState {
   S_IDLE = 0,
   S_WAIT_TASK,
   S_IN_TASK,
   S_FILTERS,
   S_MULTI_SCORER
};

constexpr size_t WA_SCORER_CODE_LEN = 32;
constexpr size_t WA_SHORT_SCORER_LEN = 4;
constexpr size_t WA_TASK_BRIEF = 512;
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
      char taskHandPBN[WA_HAND_LEN];
      char secLongName[WA_SECONDARY_LNAME_LEN];
      char freqTitleFormat[WA_TASK_NANE_LEN];
      char mulScorerSourceCode[WA_TASK_BRIEF];
      size_t sizeMulScorerSourceCode = 0;
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
      int  fixedHand = -1; // or NORTH or WEST
   } deck;

   // solving
   struct Solving {
      uint aimTaskCount = 20*1000;
      WA_OPERATION_MODE opMode = OPMODE_NONE;
      WA_TASK_TYPE taskType = TTYPE_NONE;
      int ddsSol = 1;
      struct Leads { // cards to lead for WPM_OPENING_LEADS
         int S=0, H=0, D=0, C=0;
      } leads;
      bool shouldSolveTwice = false;
      bool seekDecisionCompete = false;
      bool scorersSameLine = true;
      bool customSemantic = false;
      bool TheOtherIsOurs() const { return shouldSolveTwice && !seekDecisionCompete; }
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
      char txtCode[WA_SCORER_CODE_LEN];
      char txtShort[WA_SHORT_SCORER_LEN];
      Contract();
      void Init(const LineScorer& scorer);
      void CheckTheSetup(const LineScorer& scorer);
      bool IsNSLine() const { return (by == NORTH) || (by == SOUTH); }
      bool IsEWLine() const { return (by == EAST)  || (by == WEST); }
      bool IsEmpty()  const { return (goal == 0); }
   };
   struct AllLenses {
      union {// all contracts lenses together
         struct {
            Contract prim;      // our primary contract
            Contract secondary; // either our secondary contract or their contract
         };
         struct {
            Contract arrLenses[WA_MAX_LENSES];
         };
      };
      uint countLenses = 0;
      std::vector<MicroFilter> when;
      AllLenses() : when() {}
      void SimpleSecondary(struct deal& dl);
      void TrumpFillMultiLens(struct deal& dl);
      bool IsManyLenses() { return (countLenses > 2); }
      bool IsMagicFly();
   } lens;

   // adding extra marks aka post-mortem 
   struct Postmortem {
      WA_POSTM_TYPE  Type = WPM_NONE;
      int            idxHand, idxSuit;// for WPM_SUIT
      int            minHCP, maxHCP;
      int            minControls;
      twlHCP         hcpFixedHand;
      Postmortem();
      bool Is(WA_POSTM_TYPE t) { return (t == Type); }
      int  FactorFromRow(int idx);
   } postm;

   // display options
   struct Output {
      bool showHugeMatch = false;
      bool showOurOther = false;
      bool showOppResults = false;
      bool oppsAreDoubled = false, weAreDoubled = false;
      bool showMagicFly = false;
      bool showPercentages = false;
      int  rowPercentage = 0;
   } io;
   bool DisplayingOtherContract() { return io.showOppResults || io.showOurOther || solve.seekDecisionCompete; }

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
   void MakeSecondaryScrorerForBiddingLevel();
   void MarkFail() { isInitSuccess = false; }
   void MarkFail(const char* reason);
   bool IsInitFailed() { return !isInitSuccess; }
private:

   void ReadTask(class Walrus *walrus);
   void BuildMultiScorer(class Walrus* walrus);
   void ChangeOpMode(const char* line);
   void ReadTaskType(const char* line);
   void InitCardsCount();
   void ResolvePostmortemType(Walrus* walrus);
   void SetupOutputOptions();
   void ReadHandPBN(const char *line);
   void ReadLeadCards(const char *line);
   void ReadPrimaScorer(const char *line);
   void ReadSecundaScorer(const char *line);
   void DetectTwoScorers();
   void ReadPostmortemParams(char *line);
   void ReadDebugSetting(char *line);
   void ReadScaleSetting(char *line);
   EConfigReaderState FSM_DoFiltersState(char* line);
   EConfigReaderState FSM_DoMulScorerState(char* line);
   EConfigReaderState FSM_DoTaskState(char* line);
   EConfigReaderState FSM_Go2WaitTask(char* line);
   EConfigReaderState FSM_GoInsideTask(char* line);
   const char* TaskTypeText(WA_TASK_TYPE key);
   bool RecognizePostmType(const char* token);
   void AnalyzeTaskType();
   WA_TASK_TYPE DetectOneHandVariant();
   void AnnounceTask();

public:
   // keywords used in parsing
   struct Keywords {
      static char* Debug;
      static char* Delimiters;
      static char* Filters;
      static char* Hand;
      static char* Leads;
      static char* OpMode;
      static char* Postmortem;
      static char* Prima, *Secunda, *MultiScorer;
      static char* Scale;
      static char* ShowComparisons;
      static char* ShowOnAdded;
      static char* ShowOnReconstructed;
      static char* TEnd, *Imply;
      static char* TName, *TType;
      static char* VerboseCompile;
      static char* VerboseMemory;
      static char* PostmHCP;
   } key;
};

extern WaConfig config;

