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
   char  declTrump[10], declSeat[10], seatOnLead[10];
   MiniUI();
   void Init(int trump, int first);
   void DisplayBoard(twContext* lay);
   void WaitAnyKey();
   void FillMiniRows();
   void Run();
};

// output rows: 
// -- we down, we make, a blank line
constexpr uint IO_ROW_OUR_DOWN   = 0;
constexpr uint IO_ROW_OUR_MADE   = IO_ROW_OUR_DOWN + 1;
// -- they down, they make
constexpr uint IO_ROW_CONTRACT_B = IO_ROW_OUR_MADE + 2;
constexpr uint IO_ROW_THEIRS     = IO_ROW_CONTRACT_B;
// -- comparison
constexpr uint IO_ROW_COMPARISON = IO_ROW_CONTRACT_B + 2;
constexpr uint IO_ROW_MAGIC_FLY  = IO_ROW_COMPARISON;
constexpr uint IO_ROW_SACRIFICE  = IO_ROW_COMPARISON;
// -- hcp postmortem
#define IO_ROW_HCP_START 7
#define IO_SHIFT_FOR_EXTRA_MARKS  27
#define IO_ROW_FILTERING (3  + IO_SHIFT_FOR_EXTRA_MARKS)
#define IO_ROW_SELECTED  IO_ROW_FILTERING

// output columns
constexpr uint IO_CAMP_OFF = 0;
constexpr uint IO_CAMP_PREFER_PRIMA = 1;
constexpr uint IO_CAMP_NO_DIFF = 2;
constexpr uint IO_CAMP_PREFER_SECUNDA = 3;
// -- derived
constexpr uint IO_CAMP_PREFER_SUIT = IO_CAMP_PREFER_PRIMA;
constexpr uint IO_CAMP_SAME_NT     = IO_CAMP_NO_DIFF;
constexpr uint IO_CAMP_MORE_NT     = IO_CAMP_PREFER_SECUNDA;
constexpr uint IO_CAMP_PREFER_TO_BID   = IO_CAMP_PREFER_PRIMA;
constexpr uint IO_CAMP_REFRAIN_BIDDING = IO_CAMP_PREFER_SECUNDA;

enum WA_REPORT_TYPE {
   WREPORT_NONE = 0,
   WREPORT_HCP,
   WREPORT_CONTROLS,
   WREPORT_SUIT,
};

enum WA_OPERATION_MODE {
   OPMODE_NONE = 0,
   OPMODE_FIXED_TASK,
   OPMODE_SEMI_STRAY,
   OPMODE_STRAY,
};

constexpr uint WA_CONTR_TITLE_LEN = 5;
constexpr size_t WA_SOURCE_CODE_BUF = 2 * 1024;
constexpr size_t WA_TASK_BRIEF = 1024;
struct WaConfig {
   WA_OPERATION_MODE opMode = OPMODE_NONE;
   waFileNames namesBase;

   char titleBrief[WA_TASK_BRIEF];   // title and a brief
   char titleContractPrimary[WA_CONTR_TITLE_LEN];   // kind of "2S", "6NT", our main action
   char titleContractSecondary[WA_CONTR_TITLE_LEN]; // kind of "2Sx", "PASS", maybe their contract 

   int   primGoal;  // goal tricks in our primary contract
   int   primTrump, primFirst;

   int   secGoal; // goal tricks either in our secondary contract or in their contract

   // for post-mortem:
   int   postmSuit, postmHand; 
   int   minControls;

   WA_REPORT_TYPE detailedReportType;

   // filtering
   char   sourceCodeFilters[WA_SOURCE_CODE_BUF];
   size_t sizeSourceCode, countFilters;
   std::vector<MicroFilter> filtersLoaded;

   // other text params
   char  declTrump[10], declSeat[10], seatOnLead[10], theirTrump[10];
   char  secLongName[128];

   WaConfig();
   void ReadStart(class Walrus *walrus);
   void SetupOtherContract();
private:
   bool LoadFiltersSource();
   void BuildNewFilters(class Walrus *walrus);
};

extern WaConfig config;

